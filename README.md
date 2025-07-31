# Flōs Stella Engine
![Project Logo](images_readMe/logo_Flos_Stella_1.png)

# ----------------- Setup ------------------------^

### Clangd
# PowerShell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; Copy-Item build\compile_commands.json .


### build
Executer compileProject.bat


# ----------------- Todo -------------------------
- Ajout d'un monobehaviour comme dans unity.

- Mettre en place le "Include what you use" strict (toute les includes de chaque fichier sont inclus même si obtenu d'un autre include) [refaire le tour un peu]

- Mettre en place un assets Manager qui s'occuper de tout ce qui est assets, lazy loading et abstraction entre un système shaderManager ou Models.


- Ajouter un moyen de faire une hiéarchie avec un component comme dans unity.

- ajout un moyen de lire un .scene file qui va contenir les informations de la scène pour la charger.

- Faire le caching des pipelineLayouts (pushConstants, descriptorSets) et rendre cela dynamique 

- les render systems pour mes besoins et les shaders
