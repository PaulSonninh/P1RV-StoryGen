README — StoryGen (P1RV Sujet 9 : Génération d'histoires C++ / SLM)



1\. Objectif

&nbsp;  Ce projet fournit une CLI C++ (storygen.exe) qui lance un Small Language Model (SLM) en local via llama.cpp (llama-cli.exe) afin de générer une histoire à partir d’un prompt (fichier texte), avec des paramètres de génération configurables.



\* Plateforme : Windows

\* Langage : C++

\* Backend : llama.cpp (llama-cli.exe)

\* Modèles : GGUF (ex : Mistral 7B Instruct Q4)



2\. Structure du dépôt

&nbsp;  P1RV/



\* app/            (code C++ : main.cpp)

\* CMakeLists.txt  (build de storygen)

\* llama.cpp/      (submodule llama.cpp)

\* models/         (modèles GGUF, non inclus dans le zip)

\* prompt.txt      (prompt d’entrée)

\* out/            (répertoires de build générés)



3\. Prérequis (Windows)



\* Visual Studio 2022 (Desktop development with C++)

\* CMake (via VS ou installé séparément)

\* Ninja (recommandé, via VS possible)

\* Git (si récupération avec submodule)



4\. Récupération du submodule llama.cpp (si nécessaire)

&nbsp;  Depuis la racine P1RV :

&nbsp;  git submodule update --init --recursive



5\. Modèle GGUF (à placer dans models/)

&nbsp;  Le modèle n’est pas inclus dans le zip (taille trop importante).



Exemple utilisé :

mistral-7b-instruct-v0.2.Q4\_0.gguf



À placer ici :

P1RV\\models\\mistral-7b-instruct-v0.2.Q4\_0.gguf



Remarque : la CLI accepte aussi --model <chemin> pour utiliser un autre GGUF.



6\. Build de llama.cpp (pour obtenir llama-cli.exe)

&nbsp;  Depuis la racine P1RV :

&nbsp;  cmake -S llama.cpp -B llama.cpp\\build -G Ninja -DCMAKE\_BUILD\_TYPE=Release

&nbsp;  cmake --build llama.cpp\\build



Le binaire attendu (selon config) :

P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe



7\. Build de storygen (CLI C++)

&nbsp;  Depuis la racine P1RV :



Debug :

cmake -S . -B out\\build\\x64-Debug -G Ninja -DCMAKE\_BUILD\_TYPE=Debug

cmake --build out\\build\\x64-Debug



Release :

cmake -S . -B out\\build\\x64-Release -G Ninja -DCMAKE\_BUILD\_TYPE=Release

cmake --build out\\build\\x64-Release



8\. Exécution (démo live)

&nbsp;  Démo simple + logs :

&nbsp;  out\\build\\x64-Release\\storygen.exe --verbose



Paramètres (exemples) :

out\\build\\x64-Release\\storygen.exe -n 300 -t 0.9 -p 0.95 -s 123 --verbose



Écrire la sortie dans un fichier :

out\\build\\x64-Release\\storygen.exe --out out.txt



9\. Options CLI



\* -h, --help : affiche l’aide

\* -m, --model <path> : chemin du modèle GGUF

\* -f, --prompt <path> : chemin du fichier prompt

\* -n, --n-predict <int> : nombre de tokens à générer

\* -s, --seed <int> : seed (reproductibilité)

\* -t, --temp <float> : température (créativité)

\* -p, --top-p <float> : top-p (noyau de probas)

\* --verbose : affiche les chemins résolus + la commande exécutée

\* --out <path> : écrit stdout/stderr dans un fichier

\* --single-turn : (par défaut) 1 prompt → 1 réponse → exit



10\. Prompt d’exemple

&nbsp;   Le fichier prompt.txt contient un prompt simple.

&nbsp;   Exécution attendue : chargement du modèle, génération du texte, puis fin propre (Exiting...).



11\. Dépannage rapide



12\. “llama-cli introuvable”

&nbsp;   Vérifier :

&nbsp;   P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe

&nbsp;   Sinon recompiler llama.cpp en Release.



13\. “modèle introuvable”

&nbsp;   Vérifier la présence du .gguf dans models/ ou lancer :

&nbsp;   storygen.exe --model "C:...\\monmodele.gguf"



14\. Le programme ne se termine pas

&nbsp;   Assurer le mode single-turn (par défaut). Si modifié, relancer avec --single-turn.



15\. Problèmes x86/x64 (link / libs)

&nbsp;   Utiliser un terminal “Developer PowerShell for VS 2022” et vérifier la toolchain cible x64.



