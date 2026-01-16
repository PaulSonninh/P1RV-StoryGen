```md

\# StoryGen — P1RV Sujet 9 (Génération d'histoires C++ / SLM)



\## 🎯 Objectif

Ce projet propose une \*\*CLI C++\*\* (`storygen.exe`) qui lance un \*\*Small Language Model (SLM)\*\* en local via \*\*llama.cpp\*\* afin de générer une histoire à partir d’un prompt (fichier texte), avec paramètres de génération configurables.



\- Plateforme : \*\*Windows\*\*

\- Langage : \*\*C++\*\*

\- Backend SLM : \*\*llama.cpp\*\* (`llama-cli.exe`)

\- Modèles : \*\*GGUF\*\* (ex: Mistral 7B Instruct Q4)



---



\## 📁 Structure du dépôt

```



P1RV/

├─ app/                  # code C++ (main.cpp)

├─ CMakeLists.txt        # build de storygen

├─ llama.cpp/            # submodule llama.cpp

├─ models/               # modèles GGUF (non inclus dans le zip)

├─ prompt.txt            # prompt d'entrée (exemple)

└─ out/                  # répertoires de build (générés)



````



---



\## ✅ Prérequis (Windows)

\- \*\*Visual Studio 2022\*\* (Desktop development with C++)

\- \*\*CMake\*\* (via VS ou installé séparément)

\- \*\*Ninja\*\* (recommandé, via VS possible)

\- Git (si tu récupères le dépôt avec submodule)



---



\## 🔁 Récupération du submodule llama.cpp (si nécessaire)

Si `llama.cpp/` est un submodule :



```bat

git submodule update --init --recursive

````



---



\## 📦 Modèle GGUF (à placer dans `models/`)



Le modèle \*\*n’est pas inclus\*\* dans le zip (taille trop importante).



1\. Télécharger un modèle GGUF (exemple utilisé durant le projet) :



\* `mistral-7b-instruct-v0.2.Q4\_0.gguf`



2\. Le placer ici :



```

P1RV\\models\\mistral-7b-instruct-v0.2.Q4\_0.gguf

```



> Remarque : la CLI accepte aussi `--model <path>` si tu veux utiliser un autre GGUF.



---



\## 🛠️ Build de llama.cpp (pour obtenir llama-cli.exe)



Depuis la racine `P1RV` :



```bat

cmake -S llama.cpp -B llama.cpp\\build -G Ninja -DCMAKE\_BUILD\_TYPE=Release

cmake --build llama.cpp\\build

```



Après compilation, tu dois avoir (selon config) :



```

P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe

```



---



\## 🧱 Build de storygen (CLI C++)



Depuis la racine `P1RV` :



\### Debug



```bat

cmake -S . -B out\\build\\x64-Debug -G Ninja -DCMAKE\_BUILD\_TYPE=Debug

cmake --build out\\build\\x64-Debug

```



\### Release



```bat

cmake -S . -B out\\build\\x64-Release -G Ninja -DCMAKE\_BUILD\_TYPE=Release

cmake --build out\\build\\x64-Release

```



---



\## ▶️ Exécution (démo live)



\### Démo simple + logs



```bat

out\\build\\x64-Release\\storygen.exe --verbose

```



\### Paramètres (exemples)



```bat

out\\build\\x64-Release\\storygen.exe -n 300 -t 0.9 -p 0.95 -s 123 --verbose

```



\### Écrire la sortie dans un fichier



```bat

out\\build\\x64-Release\\storygen.exe --out out.txt

```



---



\## 🧰 Options CLI



\* `-h, --help` : affiche l’aide

\* `-m, --model <path>` : chemin du modèle GGUF

\* `-f, --prompt <path>` : chemin du fichier prompt

\* `-n, --n-predict <int>` : nombre de tokens à générer

\* `-s, --seed <int>` : seed (reproductibilité)

\* `-t, --temp <float>` : température (créativité)

\* `-p, --top-p <float>` : top-p (noyau de probas)

\* `--verbose` : affiche les chemins résolus + commande exécutée

\* `--out <path>` : écrit stdout/stderr dans un fichier

\* `--single-turn` : (par défaut) 1 prompt → 1 réponse → exit



---



\## 🧪 Prompt d’exemple



Le fichier `prompt.txt` contient un prompt simple.

Exemple attendu (résumé) :



\* `Loading model...`

\* génération d’une histoire

\* `Exiting...` (fin propre grâce au mode single-turn)



---



\## 🧨 Dépannage rapide



\### 1) “llama-cli introuvable”



Vérifie que le binaire existe :



```

P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe

```



Sinon recompile llama.cpp en Release.



\### 2) “modèle introuvable”



Vérifie la présence du `.gguf` dans `models/`, ou lance avec :



```bat

storygen.exe --model "C:\\...\\monmodele.gguf"

```



\### 3) Le programme ne se termine pas



Assure-toi d’être en mode \*\*single-turn\*\* (par défaut).

Si tu as modifié le code, repasse en `--single-turn`.



\### 4) Problèmes x86/x64 (link / libs)



Utiliser un terminal “Developer PowerShell for VS 2022” et vérifier que la toolchain cible est bien \*\*x64\*\*.



---



\## 🧾 Soutenance (infos)



\* Projet : \*\*P1RV – Sujet 9 : Génération d’histoires C++ / SLM\*\*

\* Travail en autonomie (1 étudiant)

\* Temps estimé : \*\*~15h\*\*



```



---



Si tu veux, je peux aussi te générer un \*\*ZIP “rendu”\*\* recommandé (structure + fichiers à inclure / exclure), et/ou t’écrire une version \*\*README courte\*\* (1 page max) spéciale dépôt Hippocampus.

::contentReference\[oaicite:0]{index=0}

```



