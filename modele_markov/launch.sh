#!/bin/bash



if [ $# -ge 1 ]
then
	# Déclaration des variables globales
	
	dossier_partitions_defaut="test/partitions"
	fichiers_entree=""
	redirection_sortie="/dev/fd/1" # sortie standard
	type_improvisations=""
	
	fichier_sortie=""
	date=$(date "+%d%m%Y_%H%M%S")
	
	# Traitement des arguments
	
	while [ $# -gt 0 ]
	do
		if [[ $1 == "--"* ]]
		then
			if [ "$1" == "--notraces" ]
			then
				redirection_sortie="/dev/null"
			elif [ "$1" == "--impro-random" ]
			then
				type_improvisations=$type_improvisations"random "
			elif [ "$1" == "--impro-contraintes" ]
			then
				type_improvisations=$type_improvisations"contraintes "
			elif [ "$1" == "--impro-markov" ]
			then
				type_improvisations=$type_improvisations"markov "
			else
				echo -e "# Option $1 inconnue"
				echo -e "# Options :"
				echo -e "\t--impro-random\t\tGénérer, à partir de la partition, une mélodie totalement aléatoire"
				echo -e "\t--impro-contraintes\tGénérer, à partir de la partition, une mélodie aléatoire sous contraintes"
				echo -e "\t--impro-markov\t\tGénérer, à partir de la partition, une mélodie basée uniquement sur le modèle de Markov"
				echo -e "\t--notraces\t\tMasquer la sortie lors de l'utilisation des programmes"
				
				exit 2
			fi
		else
			if [ -e $1 ]
			then
				fichiers_entree="$fichiers_entree$1 "
			else
				fichier=$(echo $1 | awk -F / '{print $NF}')
				recherche_fichier=$(find $dossier_partitions_defaut -name $fichier)
				if [ "$recherche_fichier" == "" ]
				then
					echo -e "# Fichier $1 introuvable"
					
					exit 3
				else
					fichiers_entree="$fichiers_entree$recherche_fichier "
				fi
			fi
		fi
		shift
	done
	
	# Extraction de la mélodie des partitions données en entrée
	
	melodies=""
	
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
		
		melodie="test/melodies/$tmp-$date.xml"
		melodies="$melodies$melodie "
		
		# Extraction de la mélodie de la partition
		
		echo -e "\n# Extraction de la mélodie de la partition $partition dans $melodie"
		
		if [ "$format" == "midi" ]
		then
			./bin/epurer_midi.exe $partition $melodie > $redirection_sortie
		elif [ "$format" == "musicxml" ]
		then
			./bin/epurer_musicxml.exe $partition $melodie > $redirection_sortie
		fi
	done
	
	# Modélisation des mélodies extraites

	modelisation="test/modelisations/$fichier_sortie-$date.xml"
	
	echo -e "\n# Modélisation de la (des) mélodie(s) extraite(s) dans $modelisation"
	
	./bin/modeliser.exe $melodies $modelisation > $redirection_sortie 
	
	# Génération d'une mélodie improvisée à partir des contraintes des mélodies ou des mélodies extraites
	
	if [ "$type_improvisations" != "" ]
	then
		echo -e "\n# Génération d'une mélodie improvisée"
		
		echo -e "\nNombre de notes à générer : "
		read nombre_notes
		
		IFS=' ' read -r -a array <<< "$type_improvisations"
	
		for type_impro in "${array[@]}"
		do
			# Improvisation à partir des contraintes des mélodies ou des mélodies extraites
		
			improvisation="test/improvisations/$fichier_sortie-$date-impro-$type_impro.xml"
		
			if [ "$type_impro" == "random" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir des contraintes de la (des) mélodie(s) dans $improvisation"
				./lib/generateur_aleatoire_sous_contraintes/random -o $improvisation -c $modelisation -n 20 -g 50 > $redirection_sortie
			elif [ "$type_impro" == "contraintes" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir des contraintes de la (des) mélodie(s) dans $improvisation"
				./lib/generateur_aleatoire_sous_contraintes/rmg -o $improvisation -c $modelisation -n $nombre_notes -g 50 > $redirection_sortie
			elif [ "$type_impro" == "markov" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir de la (des) mélodie(s) dans $improvisation"
				./bin/improviser.exe $nombre_notes $melodies $improvisation > $redirection_sortie
			fi
			
			# Modélisation de l'improvisation obtenue
			
			modelisation_impro="test/modelisations/$fichier_sortie-$date-impro-$type_impro.xml"
			
			echo -e "\n# Modélisation de l'improvisation obtenue dans $modelisation_impro"
			
			./bin/modeliser.exe $improvisation $modelisation_impro > $redirection_sortie
			
			# Comparaison de l'improvisation obtenue avec les mélodies originales
			
			comparaison="test/comparaisons/$fichier_sortie-$fichier_sortie-impro-$type_impro-$date.txt"
			
			echo -e "\n# Comparaison de l'improvisation obtenue avec la (les) mélodie(s) originale(s)"
			
			./bin/comparer.exe $modelisation $modelisation_impro $comparaison > $redirection_sortie
		done
	fi
	
	# Fin du script
	
	echo -e ""
	
else
	echo -e "# Utilisation : ./launch.sh partition [OPTION]"

	echo -e "# Formats acceptés :"
	echo -e "\tMIDI\t\t.mid"
	echo -e "\tMusicXML\t.xml"
	
	echo -e "# Options :"
	echo -e "\t--impro-random\t\tGénérer, à partir de la partition, une mélodie totalement aléatoire"
	echo -e "\t--impro-contraintes\tGénérer, à partir de la partition, une mélodie aléatoire sous contraintes"
	echo -e "\t--impro-markov\t\tGénérer, à partir de la partition, une mélodie basée uniquement sur le modèle de Markov"
	echo -e "\t--notraces\t\tMasquer la sortie lors de l'utilisation des programmes"
	
	exit 1
fi

exit 0


