#!/bin/bash



if [ $# -ge 1 ]
then
	fichiers_entree=""
	option_sortie=""
	
	# Traitement des arguments
	
	while [ $# -gt 0 ]
	do
		if [[ $1 == "--"* ]]
		then
			if [ "$1" == "--notraces" ]
			then
				option_sortie=" > /dev/null"
			else
				echo -e "# Option $1 inconnue"
				echo -e "# Options :"
				echo -e "\t--notraces\tMasquer la sortie lors de l'utilisation des programmes"
				
				exit 2
			fi
		else
			fichiers_entree="$fichiers_entree$1 "
		fi
		shift
	done
	
	# Extraction de la mélodie des partitions données en entrée
	
	echo -e $fichiers_entree"|"
	
	melodies=""
	fichier_sortie=""
	
	IFS=' ' read -r -a array <<< "$fichiers_entree"
	
	for fichier in "${array[@]}"
	do
		# Détection du format de la partition
		
		partition=$fichier
		
		if [[ $partition == *".mid" ]]
		then
			tmp=$(echo $partition | sed -e 's/.mid$//')
			format="midi"
		elif [[ $partition == *".xml" ]]
		then
			tmp=$(echo $partition | sed -e 's/.xml$//')
			format="musicxml"
		else
			echo -e "# Format musical $fichier non pris en charge"
			echo -e "# Formats acceptés :"
			echo -e "\tMIDI\t\t.mid"
			echo -e "\tMusicXML\t.xml"
		
			exit 3
		fi
		
		tmp=$(echo $tmp | awk -F / '{print $NF}')
		fichier_sortie=$fichier_sortie$tmp
		melodie="test/melodies/$tmp.xml"
		melodies="$melodies$melodie "
		
		# Extraction de la mélodie de la partition
		
		echo -e "\n# Extraction de la mélodie de la partition $partition dans $melodie"
		
		if [ "$format" == "midi" ]
		then
			./bin/epurer_midi.exe $partition $melodie > /dev/null
		elif [ "$format" == "musicxml" ]
		then
			./bin/epurer_musicxml.exe $partition $melodie > /dev/null
		fi
	done
	
	date=$(date -I)
	modelisation="test/modelisations/$fichier_sortie-$date.xml"
	improvisation="test/improvisations/$fichier_sortie-$date.xml"
	modelisation_impro="test/modelisations/$fichier_sortie-$date-impro.xml"
	
	# Modélisation des contraintes des mélodies extraites
	
	echo -e "\n# Modélisation des contraintes de la (des) mélodie(s) extraite(s) dans $modelisation"
	
	./bin/modeliser.exe $melodies $modelisation $option_sortie
	
	# Improvisation sur les mélodies extraites
	
	echo -e "\n# Improvisation sur la (les) mélodie(s) dans $improvisation"
	
	./lib/generateur_aleatoire_sous_contraintes/rmg -o $improvisation -c $modelisation -n 20 -g 50 $option_sortie
	
	# Modélisation des contraintes de l'improvisation obtenue
	
	echo -e "\n# Modélisation des contraintes de l'improvisation obtenue dans $modelisation_impro"
	
	./bin/modeliser.exe $improvisation $modelisation_impro $option_sortie
	
	# Vérification de l'improvisation obtenue
	
	echo -e "\n# Comparaison de l'improvisation obtenue avec la (les) mélodie(s) originale(s)"
	
	#./bin/comparer.exe $modelisation $modelisation_impro $option_sortie
	
else
	echo -e "# Utilisation : ./launch.sh partition [OPTION]"

	echo -e "# Formats acceptés :"
	echo -e "\tMIDI\t\t.mid"
	echo -e "\tMusicXML\t.xml"
	
	echo -e "# Options :"
	echo -e "\t--notraces\tMasquer la sortie lors de l'utilisation des programmes"
	
	exit 1
fi

exit 0


