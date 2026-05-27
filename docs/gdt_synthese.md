# La GDT en x86 32 bits — Synthèse

> Document de référence personnel — projet KFS / kernel from scratch

---

## 1. Qu'est-ce que la GDT, vraiment

### Le concept en une phrase

La **Global Descriptor Table** est un tableau en mémoire RAM qui décrit des **segments** (zones mémoire) en précisant pour chacun : son adresse de base, sa taille (limite), et ses droits d'accès (lecture/écriture/exécution, niveau de privilège ring 0–3).

### Pourquoi elle existe

Introduite avec le 80286 (1982) pour permettre la **protection mémoire** : le CPU vérifie à chaque accès mémoire si le segment utilisé autorise l'opération. Avant ça (8086), aucune protection : tout programme pouvait écrire n'importe où.

### En x86 64 bits

Quasi-vestigiale : la segmentation est désactivée, seule la **paging** assure la protection. La GDT reste nécessaire (le CPU l'exige) mais les segments couvrent toute la mémoire ("flat model"). En 32 bits, c'est techniquement actif mais on utilise aussi un flat model par convention.

---

## 2. Distinction fondamentale : entrées GDT ≠ registres de segment

C'est LE point qui rend la GDT mystérieuse pour beaucoup. À bien intégrer.

### Deux mondes différents

**Côté mémoire (la GDT) :** un tableau avec autant d'entrées qu'on veut (max 8192). Chaque entrée définit **un segment potentiel**.

**Côté CPU :** **6 registres de segment** physiques, gravés dans le silicium :

| Registre | Rôle |
|----------|------|
| **CS** | Code Segment — d'où viennent les instructions exécutées |
| **DS** | Data Segment — accès data par défaut |
| **SS** | Stack Segment — push/pop/call/ret |
| **ES** | Extra Segment — historique (`rep movs`) |
| **FS** | Usage général (souvent TLS) |
| **GS** | Usage général |

### Analogie du trousseau de clés

- La GDT = un **trousseau** avec N clés (entrées)
- Les registres de segment = 6 **mains** du CPU
- Chaque main tient une clé à un instant donné
- Plusieurs mains peuvent tenir la même clé
- Des clés peuvent rester sur le trousseau sans être tenues (cas des entrées user en mode kernel)

### Conséquence pratique

Dans un kernel typique avec 7 entrées GDT :
- 5 mains (DS, ES, FS, GS) tiennent la clé `0x10` (kernel data)
- 1 main (CS) tient `0x08` (kernel code)
- 1 main (SS) tient `0x18` (kernel stack)
- Les 3 entrées user (`0x20`, `0x28`, `0x30`) existent sur le trousseau mais aucune main ne les tient — elles seront prises lors du passage en ring 3.

---

## 2bis. Les 6 registres de segment en profondeur

Comprendre ce que **fait réellement** chaque registre éclaire toute la suite (IDT, syscalls, paging, TLS). Chacun a un rôle implicite : certaines instructions CPU utilisent automatiquement un segment particulier sans qu'on le précise.

### CS — Code Segment

**Rôle :** détermine d'où viennent les instructions exécutées par le CPU.

À chaque instruction, le CPU calcule l'adresse de l'instruction comme `CS.base + EIP`. En flat model, `CS.base = 0`, donc l'adresse physique est juste EIP.

**Spécificités :**
- **CPL (Current Privilege Level)** = les 2 bits bas de CS (= RPL du sélecteur CS). Quand CS = `0x08`, CPL = 0 → ring 0. Quand CS = `0x23`, CPL = 3 → ring 3.
- **Le CPU lit CS à chaque fetch d'instruction.** C'est le segment "le plus chaud".
- **Ne peut PAS être chargé par `mov`.** Modifié uniquement par `jmp far`, `call far`, `ret far`, `iret`, ou les interruptions.
- L'entrée GDT pointée doit avoir le bit **E (Executable) = 1**, sinon #GP fault.

**Cas d'usage moderne :** distinguer le ring 0 du ring 3. C'est le mécanisme central de protection : un programme user a CS avec RPL=3, et toute tentative d'instruction privilégiée (`cli`, `hlt`, `lgdt`...) déclenche une exception #GP.

### DS — Data Segment

**Rôle :** segment par défaut pour la plupart des accès mémoire à des données.

Quand tu fais `mov eax, [0x1000]`, le CPU calcule l'adresse comme `DS.base + 0x1000`. Tu peux préfixer pour forcer un autre segment (`mov eax, es:[0x1000]`), mais sans préfixe, c'est DS.

**Spécificités :**
- **Segment data par défaut** pour toutes les instructions sauf push/pop (qui utilisent SS) et les accès aux strings (qui utilisent ES en destination).
- L'entrée pointée doit avoir **E=0** (data) et idéalement **RW=1** (sinon segment read-only).
- DPL du segment ≤ CPL pour qu'on puisse y accéder en lecture/écriture.

**Cas d'usage moderne :** en flat model, DS et ES sont identiques. La distinction est historique.

### SS — Stack Segment

**Rôle :** segment utilisé par toute opération de stack — `push`, `pop`, `call`, `ret`, `enter`, `leave`, et tout accès via `[esp]` ou `[ebp]`.

Quand tu fais `push eax`, le CPU calcule l'adresse cible comme `SS.base + ESP`, et y écrit la valeur après avoir décrémenté ESP.

**Spécificités :**
- **Doit toujours être valide.** Si SS pointe vers une entrée invalide ou avec mauvais privilèges, presque toute instruction plante (puisque toute interruption pousse sur la stack).
- **`mov ss, ax` désactive automatiquement les interruptions pour l'instruction suivante.** Mécanisme de sécurité historique : sans ça, une IRQ pourrait arriver entre `mov ss, ax` et `mov esp, ...`, et le CPU aurait un SS valide avec un ESP incohérent → crash. C'est pour ça qu'on enchaîne traditionnellement :
  ```asm
  mov ax, 0x18
  mov ss, ax       ; interruptions désactivées juste après
  mov esp, <new>   ; cette instruction est protégée
  ```
- **DPL du segment SS doit être strictement égal à CPL.** Pas ≤, **égal**. C'est une particularité de SS : on ne peut pas charger un SS de DPL différent du CPL courant. C'est pour ça qu'un changement de ring entraîne forcément un changement de SS — c'est géré automatiquement par le mécanisme de TSS.

**Cas d'usage moderne :** différencier la stack kernel de la stack user. En ring 0, SS pointe vers `0x18` (kernel stack). En ring 3, SS pointe vers `0x33` (user stack). La transition entre les deux passe par la TSS.

### ES — Extra Segment

**Rôle historique :** destination des instructions de manipulation de strings (`movs`, `stos`, `cmps`, `scas`).

Quand tu fais `rep movsb` (copie de bytes), DS:ESI est la source et **ES:EDI** est la destination. C'est le seul cas où le CPU utilise ES sans préfixe explicite.

**Spécificités :**
- Aucune autre instruction n'utilise ES automatiquement.
- En flat model, on le met aux mêmes valeurs que DS pour pouvoir faire des `rep movs` qui copient n'importe où.

**Cas d'usage moderne :** quasi-aucun. On le configure comme DS et on l'oublie. Les instructions string modernes (utilisées par `memcpy` etc.) marchent avec ES = DS.

### FS — Extra General-Purpose Segment

**Rôle d'origine :** rien de spécifique. Introduit avec le 80386 comme "segment supplémentaire de plus" sans rôle implicite défini par Intel.

**Spécificités :**
- **Aucune instruction ne l'utilise par défaut.** Pour utiliser FS, il faut explicitement préfixer : `mov eax, fs:[0x10]`.
- Comme ES, n'a pas de contrainte particulière.

**Cas d'usage moderne (très important !) :**
- **Windows 32 bits** : FS pointe vers le **Thread Information Block (TIB)** de chaque thread. `fs:[0x18]` = adresse du TIB lui-même, `fs:[0x30]` = adresse du PEB. C'est comme ça que le code Windows accède aux données par thread.
- **Linux 32 bits** : utilisé pour **Thread-Local Storage (TLS)** avec glibc. Les variables `__thread` y sont accédées.
- **Stack canaries** sur Linux : `mov eax, fs:0x14` charge la valeur "canary" pour la protection contre les buffer overflows.

C'est pour ça que les segments FS/GS, qui semblaient inutiles, ont retrouvé une utilité moderne : ils servent de **pointeurs par thread**, permettant d'avoir des données distinctes pour chaque thread sans avoir à passer le pointeur explicitement.

### GS — Extra General-Purpose Segment

**Rôle d'origine :** identique à FS. Encore un segment de plus, sans rôle implicite.

**Cas d'usage moderne :**
- **Windows 64 bits** : GS pointe vers le TIB (en 32 bits c'était FS, en 64 bits c'est GS).
- **Linux 64 bits** : GS est utilisé par le kernel via `swapgs`. Cette instruction échange le contenu du registre GS entre kernel et user à chaque syscall, permettant au kernel d'avoir son propre pointeur "per-CPU" sans collision avec celui de l'user.
- **macOS** : utilise GS pour le TLS user.

### Tableau récapitulatif

| Registre | Utilisé implicitement par | Usage typique kernel | Usage moderne |
|----------|---------------------------|----------------------|---------------|
| **CS** | Toute instruction (fetch) | Détermine le ring (CPL) | Protection ring 0/3 |
| **DS** | Accès data par défaut | Lecture/écriture mémoire | Flat = ES en flat model |
| **SS** | push/pop/call/ret/enter/leave, [esp], [ebp] | Stack courante | Changé à chaque transition ring |
| **ES** | rep movs / stos / cmps (destination) | Identique à DS | Quasi-inutilisé |
| **FS** | Rien (préfixe obligatoire) | Identique à DS | TLS (Linux 32), TIB (Win 32), stack canary |
| **GS** | Rien (préfixe obligatoire) | Identique à DS | TIB (Win 64), per-CPU kernel (Linux 64) |

### Le 7ᵉ "registre" : TR (Task Register)

Mentionnons-le ici pour cohérence : le CPU a aussi un registre **TR** qui pointe vers une entrée GDT spéciale (une TSS, Task State Segment, de type "système" avec S=0). Chargé avec `ltr`, il sert lors des transitions de privilège pour récupérer la nouvelle stack (`ss0`, `esp0`).

Conceptuellement, c'est une 7ᵉ "main du CPU" qui tient sa propre clé sur le trousseau GDT.

### Le 8ᵉ : LDTR (Local Descriptor Table Register)

Pointe vers une **LDT** (Local Descriptor Table), table similaire à la GDT mais "locale" à une tâche. Sélecteurs avec TI=1 indexent dans la LDT plutôt que dans la GDT.

**En pratique aujourd'hui : personne ne l'utilise.** Les OS modernes (Linux, Windows, BSD, macOS) ignorent la LDT. Tu peux faire pareil dans ton kernel.

### Conséquences pour la conception du kernel

**Ce que tu dois retenir pour KFS :**

1. **CS** : seul ce registre détermine le ring courant. Le passage ring 3 → ring 0 (syscalls, interruptions) modifie automatiquement CS.

2. **SS** : changé en même temps que CS lors des transitions de ring. La TSS contient l'info "vers quel SS basculer quand on passe en ring 0".

3. **DS, ES** : configurés une fois au boot, jamais touchés ensuite (sauf retour ring 3 où on les met aux sélecteurs user).

4. **FS, GS** : tu peux les ignorer complètement au début. Plus tard, si tu implémentes du multithreading, ils deviendront utiles pour le TLS.

5. **TR** : à configurer **une seule fois** quand tu mets en place la TSS (étape post-IDT, juste avant le passage en ring 3).

---

## 3. Format d'une entrée GDT (8 octets)

### Le format Frankenstein (héritage 286 → 386)

```
Bits  63        56 55  52 51    48 47       40 39        32 31              16 15            0
     ┌───────────┬───────┬────────┬───────────┬────────────┬──────────────────┬───────────────┐
     │Base[31:24]│Flags  │Lim[19:16]│ Access  │Base[23:16] │  Base[15:0]      │ Limit[15:0]   │
     └───────────┴───────┴────────┴───────────┴────────────┴──────────────────┴───────────────┘
```

La base est coupée en 3 morceaux, la limite en 2. C'est moche mais c'est ainsi.

### Octet Access (bits 47-40)

```
  7   6   5   4   3   2   1   0
┌───┬───────┬───┬───┬───┬───┬───┐
│ P │  DPL  │ S │ E │DC │ RW│ A │
└───┴───────┴───┴───┴───┴───┴───┘
```

| Bit | Nom | Description |
|-----|-----|-------------|
| **P** | Present | 1 = segment présent (toujours 1 pour des segments actifs) |
| **DPL** | Descriptor Privilege Level | 00 = ring 0 (kernel), 11 = ring 3 (user) |
| **S** | Descriptor type | 1 = code/data, 0 = système (TSS, gate...) |
| **E** | Executable | 1 = code, 0 = data |
| **DC** | Direction/Conforming | Toujours 0 en pratique |
| **RW** | Read/Write | Code : lisible ? Data : inscriptible ? → mettre 1 |
| **A** | Accessed | Mis automatiquement à 1 par le CPU |

### Nibble Flags (bits 55-52)

```
  3   2   1   0
┌───┬───┬───┬───┐
│ G │ DB│ L │Rsv│
└───┴───┴───┴───┘
```

| Bit | Description |
|-----|-------------|
| **G** | Granularité : 0 = octets, 1 = pages de 4 KiB → mettre 1 pour 4 GiB |
| **DB** | 0 = 16 bits, 1 = 32 bits → 1 pour nous |
| **L** | Long mode (64 bits) → 0 en 32 bits |
| **Rsv** | 0 |

### Valeurs typiques (flat model 32 bits)

| Segment | Base | Limit | Access | Flags | = 0x... |
|---------|------|-------|--------|-------|---------|
| Kernel code | 0 | 0xFFFFF | 0x9A | 0xC | `0xCF9A` |
| Kernel data | 0 | 0xFFFFF | 0x92 | 0xC | `0xCF92` |
| User code | 0 | 0xFFFFF | 0xFA | 0xC | `0xCFFA` |
| User data | 0 | 0xFFFFF | 0xF2 | 0xC | `0xCFF2` |

---

## 4. Le sélecteur de segment (16 bits)

Ce qu'on met dans CS, DS, etc.

```
  15                3   2   1   0
 ┌────────────────────┬───┬───────┐
 │      Index         │TI │  RPL  │
 └────────────────────┴───┴───────┘
```

| Champ | Description |
|-------|-------------|
| **Index** | Numéro d'entrée dans la table (× 8 = offset en octets) |
| **TI** | Table Indicator : 0 = GDT, 1 = LDT (ignorer la LDT) |
| **RPL** | Requested Privilege Level : 00 ring 0, 11 ring 3 |

### Table des sélecteurs (convention KFS)

| Index | Sélecteur ring 0 | Sélecteur ring 3 | Usage |
|-------|------------------|------------------|-------|
| 0 | `0x00` | — | Null descriptor (obligatoire) |
| 1 | `0x08` | — | Kernel code |
| 2 | `0x10` | — | Kernel data |
| 3 | `0x18` | — | Kernel stack |
| 4 | `0x20` | `0x23` | User code |
| 5 | `0x28` | `0x2B` | User data |
| 6 | `0x30` | `0x33` | User stack |

---

## 5. Le chargement de la GDT — assembleur

### Le rôle de `gdt_flush`

Quand GRUB nous laisse la main, le CPU est :
- En mode protégé (PE=1 dans CR0)
- Avec une GDT GRUB déjà chargée (à `0x10b0` typiquement)
- Avec des sélecteurs GRUB dans CS, DS, etc.

Objectif de `gdt_flush` :
1. **Faire pointer GDTR vers NOTRE GDT** (via `lgdt`)
2. **Recharger les 6 registres de segment** avec des sélecteurs valides pour notre table

### Code annoté

```asm
.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    movl 4(%esp), %eax     # récupère l'argument (adresse de gp)
    lgdt (%eax)            # CPU : "ta GDT est ici maintenant"

    # Recharge les data segments (DS/ES/FS/GS = kernel data)
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    # Recharge SS séparément (kernel stack)
    movw $0x18, %ax
    movw %ax, %ss

    # Recharge CS — IMPOSSIBLE avec mov, OBLIGATOIRE en far jump
    ljmp $0x08, $.flush
.flush:
    ret
```

### Les trois mécanismes clés

#### `lgdt` — Load GDT

Lit 6 octets à l'adresse donnée et les charge dans le registre **GDTR** du CPU :
- 2 octets : limit (taille - 1)
- 4 octets : base address

Format identique à la struct `gdt_ptr` (avec `__attribute__((packed))` côté C).

**À partir de cet instant, le CPU sait où est la nouvelle GDT.**

#### `mov segment, sélecteur` — recharger un registre de segment

Quand on fait `mov %ax, %ds`, le CPU :
1. Lit l'index dans le sélecteur
2. Va lire l'entrée correspondante **dans la GDT actuelle**
3. Stocke base/limit/access dans le **cache interne** du registre de segment

**Sans ce rechargement, le cache de l'ancienne GDT (GRUB) reste actif** même si GDTR pointe ailleurs.

#### `ljmp` — long jump (le piège)

CS **ne peut PAS** être chargé avec `mov`. Le CPU refuse.

Le seul moyen de recharger CS : une instruction qui modifie le flux d'exécution (`jmp far`, `call far`, `iret`).

`ljmp $0x08, $.flush` :
1. Met `0x08` dans CS (recharge le cache CS depuis la nouvelle GDT)
2. Saute à l'étiquette `.flush`

Sans `ljmp`, CS garde l'ancien sélecteur GRUB et le cache associé. Ça peut marcher par chance si les segments GRUB sont compatibles (flat model), mais c'est faux et instable.

---

## 6. La partie cachée des registres de segment

Chaque registre de segment a deux parties :

```
Partie VISIBLE (16 bits) :  sélecteur lu/écrit
Partie CACHÉE (64 bits)  :  copie du descripteur — base, limit, access, flags
```

Le cache est rempli **au moment du chargement** (`mov ds, ax` ou `ljmp`). Ensuite, tous les accès via ce registre utilisent le cache, **sans relire la GDT**. C'est ce qui rend la segmentation rapide.

C'est aussi pour ça qu'on doit recharger les registres après `lgdt` : le cache est "périmé" mais le CPU ne le sait pas tout seul.

QEMU le montre dans `info registers` :
```
CS = 0008 00000000 ffffffff 00cf9a00 DPL=0 CS32 [-R-]
     ↑    ↑        ↑        ↑
   visible base    limit    flags+access (partie cachée)
```

---

## 7. Vérifications dans QEMU

### Monitor QEMU (`-monitor stdio`)

```
(qemu) info registers
```

Vérifier :
- `CS = 0008` (kernel code)
- `DS = ES = FS = GS = 0010` (kernel data)
- `SS = 0018` (kernel stack)
- `GDT = <adresse> <limit>` avec limit cohérente (`0x37` pour 7 entrées)
- `CPL = 0` (en ring 0)

### Dump de la GDT

```
(qemu) xp /14wx <adresse_GDT>     # 14 words = 56 octets = 7 entrées
(qemu) xp /56bx <adresse_GDT>     # octet par octet
```

### Avec GDB (`qemu -s -S` + `i686-elf-gdb`)

```
(gdb) target remote :1234
(gdb) break kernel_main
(gdb) continue
(gdb) print/x gdt
(gdb) x/8bx &gdt[1]
(gdb) monitor info registers
```

---

## 8. Comment vérifier que c'est BIEN notre GDT (et pas celle de GRUB)

**Piège classique** : les sélecteurs `0x08`/`0x10` peuvent coïncider avec ceux de GRUB par convention.

**Vraie preuve** : comparer **l'adresse** dans GDTR.

| | Sans `gdt_init` | Avec `gdt_init` |
|---|---|---|
| GDT base | `0x000010b0` (zone GRUB) | `0x0021a960` (notre `.bss`) |
| GDT limit | `0x20` (5 entrées GRUB) | `0x37` (7 entrées kernel) |
| CS | `0x10` | `0x08` |
| DS | `0x18` | `0x10` |

**ATTENTION** : toujours faire `make re` (full rebuild) après modification. Un `make` incrémental peut louper la régénération de l'ISO.

---

## 9. Placement de la GDT à une adresse fixe

Trois approches pour placer la GDT à `0x800` (ou autre).

### Approche 1 — Copie manuelle (simple)

Construire la GDT dans `.bss`, puis `memcpy` à l'adresse cible, puis mettre cette adresse dans `gp.base` avant `lgdt`.

### Approche 2 — Linker script (propre, recommandée pour KFS)

Dans `linker.ld` :
```ld
.gdt 0x00000800 (NOLOAD) :
{
    *(.gdt)
}
```

Dans `gdt.c` :
```c
static uint64_t gdt[GDT_ENTRIES] __attribute__((section(".gdt")));
```

- `NOLOAD` = réserve l'adresse sans stocker dans le binaire
- `*(.gdt)` = capture tout symbole marqué section `.gdt` dans tous les `.o`
- L'attribut `__attribute__((section(".gdt")))` marque la variable

Vérifications :
```bash
i686-elf-objdump -h kfs       # voir la section .gdt à VMA 0x800
i686-elf-nm kfs | grep gdt    # voir le symbole gdt à 0x800
```

### Approche 3 — Pointeur direct (hacky)

```c
uint64_t *gdt = (uint64_t *)0x800;
```

Dangereux : aucune réservation, collisions possibles.

---

## 10. Ce qui suit

### Immédiat : IDT (Interrupt Descriptor Table)

- Même principe : table en mémoire + registre IDTR + instruction `lidt`
- Chaque entrée IDT contient un **sélecteur de segment de code** → utilise `0x08` (notre kernel code dans la GDT)
- D'où l'importance d'installer la GDT en premier

### Plus tard : TSS (Task State Segment)

- C'est **une autre entrée dans la GDT** (à l'index 7 ou 8)
- Référencée par un **7ᵉ "registre de segment"** : le **Task Register** (`TR`)
- Chargée avec `ltr` (Load Task Register)
- Nécessaire pour les transitions ring 3 → ring 0 (syscalls, interruptions depuis user)
- Contient notamment `ss0` et `esp0` : où basculer la stack lors d'un changement de privilège

### Encore plus tard : paging

- Remplace progressivement la segmentation pour la vraie protection mémoire
- La GDT devient alors "vestigiale" mais reste obligatoire

---

## 11. Anti-sèche

### Si triple fault après `gdt_init`

- Structure pas `__attribute__((packed))` → décalage des champs
- Mauvaise adresse passée à `lgdt` (oubli du `&`)
- Pas de far jump après `lgdt` (CS pointe encore sur ancienne GDT disparue)
- Ordre limit/base inversé dans `gdt_ptr`

### Si comportement bizarre mais pas de crash

- `gdt_init()` n'est pas appelée (oubli, ou `make` n'a pas recompilé)
- Le `ljmp` manque (CS pas rafraîchi, marche par coïncidence)
- Variable GDT pas `static` (free après retour de fonction)

### Réflexe debug

1. `make re` systématique entre deux tests
2. `info registers` dans QEMU : vérifier `GDT=`, `CS`, `DS`, `SS`
3. `xp /14wx <addr_GDT>` : voir le contenu brut
4. Comparer "avec" et "sans" `gdt_init()` (commenter/décommenter + `make re`)

---

## 12. Code de référence

### `gdt.h`

```c
#ifndef GDT_H
# define GDT_H

# include <stdint.h>

/* Macros de flags (style OSDev) */
# define SEG_DESCTYPE(x)  ((x) << 0x04)
# define SEG_PRES(x)      ((x) << 0x07)
# define SEG_SAVL(x)      ((x) << 0x0C)
# define SEG_LONG(x)      ((x) << 0x0D)
# define SEG_SIZE(x)      ((x) << 0x0E)
# define SEG_GRAN(x)      ((x) << 0x0F)
# define SEG_PRIV(x)     (((x) & 0x03) << 0x05)

# define SEG_DATA_RDWR    0x02
# define SEG_CODE_EXRD    0x0A

# define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                      SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                      SEG_PRIV(0)     | SEG_CODE_EXRD

# define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                      SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                      SEG_PRIV(0)     | SEG_DATA_RDWR

# define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                      SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                      SEG_PRIV(3)     | SEG_CODE_EXRD

# define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                      SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                      SEG_PRIV(3)     | SEG_DATA_RDWR

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag);
void     gdt_init(void);

#endif
```

### `gdt.c`

```c
#include "gdt.h"

#define GDT_ENTRIES 7

static uint64_t       gdt[GDT_ENTRIES];
static struct gdt_ptr gp;

extern void gdt_flush(uint32_t gdt_ptr_addr);

uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor;

    descriptor  =  limit       & 0x000F0000;
    descriptor |= (flag <<  8) & 0x00F0FF00;
    descriptor |= (base >> 16) & 0x000000FF;
    descriptor |=  base        & 0xFF000000;
    descriptor <<= 32;
    descriptor |= base  << 16;
    descriptor |= limit & 0x0000FFFF;

    return descriptor;
}

void gdt_init(void)
{
    gdt[0] = 0;                                              /* Null */
    gdt[1] = create_descriptor(0, 0x000FFFFF, GDT_CODE_PL0); /* Kernel code  - 0x08 */
    gdt[2] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL0); /* Kernel data  - 0x10 */
    gdt[3] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL0); /* Kernel stack - 0x18 */
    gdt[4] = create_descriptor(0, 0x000FFFFF, GDT_CODE_PL3); /* User code    - 0x20 */
    gdt[5] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL3); /* User data    - 0x28 */
    gdt[6] = create_descriptor(0, 0x000FFFFF, GDT_DATA_PL3); /* User stack   - 0x30 */

    gp.limit = sizeof(gdt) - 1;
    gp.base  = (uint32_t)&gdt;

    gdt_flush((uint32_t)&gp);
}
```

### `gdt_flush.s`

```asm
.global gdt_flush
.type gdt_flush, @function

gdt_flush:
    movl 4(%esp), %eax
    lgdt (%eax)

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movw $0x18, %ax
    movw %ax, %ss

    ljmp $0x08, $.flush
.flush:
    ret
```

---

## Ressources externes

- [OSDev Wiki — GDT](https://wiki.osdev.org/Global_Descriptor_Table)
- [OSDev Wiki — GDT Tutorial](https://wiki.osdev.org/GDT_Tutorial)
- [OSDev Wiki — Memory Map (x86)](https://wiki.osdev.org/Memory_Map_(x86))
- Intel SDM Vol. 3A, chapitre 3 (Protected-Mode Memory Management)
- *Writing a Simple OS from Scratch* (Nick Blundell) — chapitre 4
