# StoryGen — P1RV Sujet 9 (Génération d’histoires C++ / SLM)

> **CLI C++ (Windows)** pour lancer un **SLM local** via **llama.cpp** (`llama-cli.exe`) et générer une histoire à partir d’un prompt.

---

## 1) Objectif

Ce projet fournit une **CLI C++** (`storygen.exe`) qui lance un **Small Language Model (SLM)** en local via **llama.cpp** (`llama-cli.exe`) afin de générer une histoire à partir d’un **prompt** (fichier texte).  
Les paramètres de génération sont configurables via la ligne de commande.

- **Plateforme :** Windows  
- **Langage :** C++  
- **Backend :** llama.cpp (`llama-cli.exe`)  
- **Modèles :** format **GGUF** (ex : Mistral 7B Instruct quantifié)

---

## 2) Structure du dépôt

Arborescence typique :

- `app/` : code C++ (ex : `main.cpp`)
- `CMakeLists.txt` : build de `storygen`
- `llama.cpp/` : submodule llama.cpp
- `models/` : modèles GGUF (**non inclus** dans le zip, trop volumineux)
- `prompt.txt` : prompt d’entrée
- `out/` : répertoires de build générés (à ne pas versionner)

---

## 3) Prérequis (Windows)

- **Visual Studio 2022** (workload **Desktop development with C++**)
- **CMake** (via Visual Studio ou installé séparément)
- **Ninja** (recommandé ; peut être fourni par VS)
- **Git** (si récupération avec submodule)
- Architecture : **x64 obligatoire** (évite les erreurs mémoire / link en x86)

---

## 4) Récupération (avec submodule llama.cpp)

```bash
git submodule update --init --recursive
```

---

## 5) Modèle GGUF (à placer dans `models/`)

Le modèle n’est **pas inclus** dans le dépôt / zip (taille trop importante).  
Télécharge un modèle **GGUF** compatible llama.cpp et place-le dans `models/`.

### Liens utiles (Hugging Face)

- Modèle “officiel” (base Instruct) :  
  https://huggingface.co/mistralai/Mistral-7B-Instruct-v0.2
- GGUF prêt pour llama.cpp (ex : Q4_0, Q5_K_M, etc.) :  
  https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.2-GGUF

### Exemple utilisé

- Fichier : `mistral-7b-instruct-v0.2.Q4_0.gguf`
- Chemin recommandé :
  - `P1RV\models\mistral-7b-instruct-v0.2.Q4_0.gguf`

---

## 6) Build de llama.cpp (pour obtenir `llama-cli.exe`)

> **Recommandé :** build Visual Studio + `-A x64` (robuste, évite les mélanges x86/x64)

### Option A — Générateur Visual Studio (x64)

```bash
rmdir /s /q llama.cpp\build
cmake -S llama.cpp -B llama.cpp\build -G "Visual Studio 17 2022" -A x64
cmake --build llama.cpp\build --config Release
```

Binaire attendu :
- `P1RV\llama.cpp\build\bin\Release\llama-cli.exe`

⚠️ Important : `llama-cli.exe` doit rester dans le même dossier que ses DLL `ggml-*.dll` (sinon erreurs de backend).

### Option B — Ninja (si terminal x64 VS)

```bash
rmdir /s /q llama.cpp\build
cmake -S llama.cpp -B llama.cpp\build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build llama.cpp\build
```

---

## 7) Build de StoryGen (CLI C++)

### Option A — Visual Studio (x64, recommandé)

```bash
rmdir /s /q out
cmake -S . -B out\vs-x64 -G "Visual Studio 17 2022" -A x64
cmake --build out\vs-x64 --config Release
```

Binaire attendu :
- `out\vs-x64\Release\storygen.exe`

### Option B — Ninja (si terminal x64 VS)

Debug :

```bash
cmake -S . -B out\build\x64-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build out\build\x64-Debug
```

Release :

```bash
cmake -S . -B out\build\x64-Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out\build\x64-Release
```

---

## 8) Quick start (copier-coller)

1) Build llama.cpp (Release, x64) :

```bash
cmake -S llama.cpp -B llama.cpp\build -G "Visual Studio 17 2022" -A x64
cmake --build llama.cpp\build --config Release
```

2) Build storygen (Release, x64) :

```bash
cmake -S . -B out\vs-x64 -G "Visual Studio 17 2022" -A x64
cmake --build out\vs-x64 --config Release
```

3) Exécution (démo) :

```bash
out\vs-x64\Release\storygen.exe --verbose
```

---

## 9) Exécution (démo)

Démo simple + logs :

```bash
storygen.exe --verbose
```

Exemples de paramètres :

```bash
storygen.exe -n 300 -t 0.9 -p 0.95 -s 123 --verbose
```

Écrire la sortie dans un fichier :

```bash
storygen.exe --out out.txt
```

---

## 10) Options CLI (selon implémentation)

- `-h`, `--help` : affiche l’aide
- `-m`, `--model <path>` : chemin du modèle GGUF
- `-f`, `--prompt <path>` : chemin du fichier prompt
- `-n`, `--n-predict <int>` : nombre de tokens à générer
- `-s`, `--seed <int>` : seed (reproductibilité)
- `-t`, `--temp <float>` : température (créativité)
- `-p`, `--top-p <float>` : top-p (nucleus sampling)
- `--verbose` : affiche chemins résolus + commande exécutée
- `--out <path>` : écrit stdout/stderr dans un fichier
- `--single-turn` : 1 prompt → 1 réponse → exit (évite le mode interactif)

---

## 11) Prompt d’exemple

Le fichier `prompt.txt` contient un prompt simple.

Exécution attendue :
- chargement du modèle
- génération du texte
- fin propre (`Exiting...`)

---

## 12) Dépannage rapide

### A) `load_backend: failed to find ggml_backend_init ... ggml-cpu.dll`
Cause fréquente : mismatch entre `llama-cli.exe` et `ggml-*.dll` (copies partielles, mélange Debug/Release).

✅ Fix :
- garder `llama-cli.exe` **avec** ses `ggml-*.dll` dans le même dossier
- clean rebuild llama.cpp en Release

### B) `ggml_backend_cpu_buffer_type_alloc_buffer: failed to allocate buffer ...`
Cause : mémoire insuffisante / contexte trop grand / ou exécution en x86.

✅ Fix :
- s’assurer d’être en **x64**
- réduire les paramètres gourmands si exposés (contexte/batch)
- vérifier que la mémoire virtuelle Windows (pagefile) est activée et assez grande

### C) Erreurs LNK / conflits x86-x64
Cause : build incohérent (x86 vs x64) ou cache CMake réutilisé.

✅ Fix :
- supprimer complètement les dossiers de build (`out/`, `llama.cpp\build\`)
- régénérer avec `-A x64`
- rebuild propre

---

## 13) Crédits

- Backend : **llama.cpp** (submodule)
- Modèle : **Mistral 7B Instruct v0.2** (téléchargé séparément en GGUF)

