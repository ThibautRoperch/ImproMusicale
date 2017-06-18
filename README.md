# Improvisation musicale à partir de partitions

## Recommandations à propos des partitions

Les partitions doivent être au format MIDI (.mid) ou MusicXML (.xml).

Le nom de ces fichiers musicaux ne doit pas contenir d'espace, ainsi que les caractères suivants :
* `&`
* `?`

Elles doivent être placées dans le dossier Partitions (directement dans le dossier ou dans un sous-dossier).

## Compilation des sources

Avant d'utiliser le générateur aléatoire de mélodies, compiler les sources avec la commande :

    cd ModeleMarkov ; make compiler_librairies ; make

## Exécution

Pour improviser sur une ou plusieurs partitions, ouvrir le fichier :

    index.html
