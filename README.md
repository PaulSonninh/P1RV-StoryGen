README — StoryGen (P1RV Sujet 9 : Génération d'histoires C++ / SLM)



Objectif

Ce projet fournit une CLI C++ (storygen.exe) qui lance un Small Language Model (SLM) en local via llama.cpp (llama-cli.exe) afin de générer une histoire à partir d’un prompt (fichier texte), avec des paramètres de génération configurables.



Plateforme : Windows



Langage : C++



Backend : llama.cpp (llama-cli.exe)



Modèles : GGUF (ex : Mistral 7B Instruct Q4)



Structure du dépôt

P1RV/



app/ (code C++ : main.cpp)



CMakeLists.txt (build de storygen)



llama.cpp/ (submodule llama.cpp)



models/ (modèles GGUF, non inclus dans le zip)



prompt.txt (prompt d’entrée)



out/ (répertoires de build générés)



Prérequis (Windows)



Visual Studio 2022 (Desktop development with C++)



CMake (via VS ou installé séparément)



Ninja (recommandé, via VS possible)



Git (si récupération avec submodule)



Récupération du submodule llama.cpp (si nécessaire)

Depuis la racine P1RV :

git submodule update --init --recursive



Modèle GGUF (à placer dans models/)

Le modèle n’est pas inclus dans le zip (taille trop importante).



Exemple utilisé :

mistral-7b-instruct-v0.2.Q4\_0.gguf



À placer ici :

P1RV\\models\\mistral-7b-instruct-v0.2.Q4\_0.gguf



Remarque : la CLI accepte aussi --model <chemin> pour utiliser un autre GGUF.



Build de llama.cpp (pour obtenir llama-cli.exe)

Depuis la racine P1RV :

cmake -S llama.cpp -B llama.cpp\\build -G Ninja -DCMAKE\_BUILD\_TYPE=Release

cmake --build llama.cpp\\build



Le binaire attendu (selon config) :

P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe



Build de storygen (CLI C++)

Depuis la racine P1RV :



Debug :

cmake -S . -B out\\build\\x64-Debug -G Ninja -DCMAKE\_BUILD\_TYPE=Debug

cmake --build out\\build\\x64-Debug



Release :

cmake -S . -B out\\build\\x64-Release -G Ninja -DCMAKE\_BUILD\_TYPE=Release

cmake --build out\\build\\x64-Release



Exécution (démo live)

Démo simple + logs :

out\\build\\x64-Release\\storygen.exe --verbose



Paramètres (exemples) :

out\\build\\x64-Release\\storygen.exe -n 300 -t 0.9 -p 0.95 -s 123 --verbose



Écrire la sortie dans un fichier :

out\\build\\x64-Release\\storygen.exe --out out.txt



Options CLI



-h, --help : affiche l’aide



-m, --model <path> : chemin du modèle GGUF



-f, --prompt <path> : chemin du fichier prompt



-n, --n-predict <int> : nombre de tokens à générer



-s, --seed <int> : seed (reproductibilité)



-t, --temp <float> : température (créativité)



-p, --top-p <float> : top-p (noyau de probas)



--verbose : affiche les chemins résolus + la commande exécutée



--out <path> : écrit stdout/stderr dans un fichier



--single-turn : (par défaut) 1 prompt → 1 réponse → exit



Prompt d’exemple

Le fichier prompt.txt contient un prompt simple.

Exécution attendue : chargement du modèle, génération du texte, puis fin propre (Exiting...).



Dépannage rapide



“llama-cli introuvable”

Vérifier :

P1RV\\llama.cpp\\build\\bin\\Release\\llama-cli.exe

Sinon recompiler llama.cpp en Release.



“modèle introuvable”

Vérifier la présence du .gguf dans models/ ou lancer :

storygen.exe --model "C:...\\monmodele.gguf"



Le programme ne se termine pas

Assurer le mode single-turn (par défaut). Si modifié, relancer avec --single-turn.



Problèmes x86/x64 (link / libs)

Utiliser un terminal “Developer PowerShell for VS 2022” et vérifier la toolchain cible x64.

