# Flōs Stella Engine
![Project Logo](images_readMe/logo_Flos_Stella_1.png)

# ----------------- Setup ------------------------^

### Clangd
# PowerShell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; Copy-Item build\compile_commands.json .


### build
Executer compileProject.bat


# ----------------- Todo ------------------------

- Mettre en place le "Include what you use" strict (toute les includes de chaque fichier sont inclus même si obtenu d'un autre include)

- Utiliser le logger à la place du std::cout

- Ajouter un moyen de faire une hiéarchie avec un component comme dans unity.

- ajout un moyen de lire un .scene file qui va contenir les informations de la scène pour la charger.

- Ajout d'un monobehaviour comme dans unity.

- peut-etre faire en sorte que le watchForChanges() n'est pas a chaque frame.

- faire la compilation des shaders automatique (gérer .glsl, .vert, .frag)

- Faire le caching des pipelineLayouts (pushConstants, descriptorSets) et rendre cela dynamique 

- faire des shaders plus dynamics

- les render systems pour mes besoins et les shaders
