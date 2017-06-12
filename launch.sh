#!/bin/bash



if [ $# -ge 1 ]
then
	# Déclaration des variables globales
	
	fichiers_entree=""
	redirection_sortie="/dev/fd/1" # sortie standard
	type_improvisations=""
	nombre_notes="-1"
	fichier_historique="ModeleMarkov/log.txt"
	
	fichier_sortie=""
	date=$(date "+%H%M%S")
	
	# Ajout de l'en-tête de l'historique des fichiers qui vont être générés
	
	date "+%d/%m/%Y %H:%M:%S" >> $fichier_historique
	
	# Traitement des arguments
	
	while [ $# -gt 0 ]
	do
		if [[ $1 == "--"* ]]
		then
			if [ "$1" == "--notraces" ]
			then
				redirection_sortie="/dev/null"
			elif [ "$1" == "--impro-aleatoire" ]
			then
				type_improvisations=$type_improvisations"aleatoire "
			elif [ "$1" == "--impro-contraintes" ]
			then
				type_improvisations=$type_improvisations"contraintes "
			elif [ "$1" == "--impro-markov" ]
			then
				type_improvisations=$type_improvisations"markov "
			elif [ "$1" == "--nombre-notes" ]
			then
				shift
				nombre_notes=$1
			else
				echo -e "# Option $1 inconnue"
				echo -e "# Options :"
				echo -e "\t--impro-aleatoire\tGénérer, à partir de la partition, une mélodie totalement aléatoire"
				echo -e "\t--impro-contraintes\tGénérer, à partir de la partition, une mélodie aléatoire sous contraintes"
				echo -e "\t--impro-markov\t\tGénérer, à partir de la partition, une mélodie basée uniquement sur le modèle de Markov"
				echo -e "\t--nombre-notes NB\tIndiquer le nombre de notes à générer par improvisation, avec NB le nombre de notes à générer"
				echo -e "\t--notraces\t\tMasquer la sortie lors de l'utilisation des programmes"
				
				exit 2
			fi
		else
			if [ -e $1 ]
			then
				fichiers_entree="$fichiers_entree$1 "
			else
				fichier=$(echo $1 | awk -F / '{print $NF}')
				dossier_partitions="ModeleMarkov/test/partitions"
				recherche_fichier=$(find $dossier_partitions -name $fichier)
				if [ "$recherche_fichier" == "" ]
				then
					echo -e "# Fichier $1 introuvable dans $dossier_partitions"
					
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
			echo -e "# Formats musicaux acceptés :"
			echo -e "\tMIDI\t\t.mid"
			echo -e "\tMusicXML\t.xml"
		
			exit 3
		fi
		
		tmp=$(echo $tmp | awk -F / '{print $NF}')
		fichier_sortie=$fichier_sortie$tmp
		
		melodie="ModeleMarkov/test/melodies/$tmp-$date.xml"
		melodies="$melodies$melodie "
		
		# Extraction de la mélodie de la partition
		
		echo -e "\n# Extraction de la mélodie de la partition $partition dans $melodie"
		
		if [ "$format" == "midi" ]
		then
			./ModeleMarkov/bin/epurer_midi.exe $partition $melodie > $redirection_sortie
		elif [ "$format" == "musicxml" ]
		then
			./ModeleMarkov/bin/epurer_musicxml.exe $partition $melodie > $redirection_sortie
		fi
		
		echo "$melodie" >> $fichier_historique
	done
	
	# Modélisation des mélodies extraites

	modelisation="ModeleMarkov/test/modelisations/$fichier_sortie-$date.xml"
	
	echo -e "\n# Modélisation de la (des) mélodie(s) extraite(s) dans $modelisation"
	
	./ModeleMarkov/bin/modeliser.exe $melodies $modelisation > $redirection_sortie
	
	echo "$modelisation" >> $fichier_historique
	
	# Génération d'une mélodie improvisée à partir des contraintes des mélodies ou des mélodies extraites
	
	IFS=' ' read -r -a array <<< "$type_improvisations"
	
	if [ ${#array[@]} -gt 0 ]
	then
		echo -e "\n# Génération des ${#array[@]} mélodies (improvisations)"
		
		if [ $nombre_notes -lt 0 ]
		then
			echo -e "\n# Nombre de notes à générer : "
			read nombre_notes
		fi
		
		if [ $nombre_notes -lt 0 ]
		then
			echo -e "# Le nombre de notes ne peut être inférieur à 0"
			
			exit 4
		fi
	
		for type_impro in "${array[@]}"
		do
			# Improvisation à partir des contraintes des mélodies ou des mélodies extraites
		
			improvisation="ModeleMarkov/test/improvisations/$fichier_sortie-$date-impro-$type_impro.xml"
		
			if [ "$type_impro" == "aleatoire" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir des contraintes de la (des) mélodie(s) dans $improvisation"
				
				./ModeleMarkov/lib/generateur_aleatoire_sous_contraintes/aleatoire -o $improvisation -c $modelisation -n 20 -g 50 > $redirection_sortie
			elif [ "$type_impro" == "contraintes" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir des contraintes de la (des) mélodie(s) dans $improvisation"
				
				./ModeleMarkov/lib/rmg -o $improvisation -c $modelisation -n $nombre_notes -g 50 -ls > $redirection_sortie
			elif [ "$type_impro" == "markov" ]
			then
				echo -e "\nImprovisation ($type_impro) à partir de la (des) mélodie(s) dans $improvisation"
				
				./ModeleMarkov/bin/improviser.exe $nombre_notes $melodies $improvisation > $redirection_sortie
			fi
			
			echo "$improvisation" >> $fichier_historique
			
			# Modélisation de l'improvisation obtenue
			
			modelisation_impro="ModeleMarkov/test/modelisations/$fichier_sortie-$date-impro-$type_impro.xml"
			
			echo -e "\n# Modélisation de l'improvisation obtenue dans $modelisation_impro"
			
			./ModeleMarkov/bin/modeliser.exe $improvisation $modelisation_impro > $redirection_sortie
			
			echo "$modelisation_impro" >> $fichier_historique
			
			# Comparaison de l'improvisation obtenue avec les mélodies originales
			
			comparaison="ModeleMarkov/test/comparaisons/$fichier_sortie-$fichier_sortie-impro-$type_impro-$date.html"
			
			echo -e "\n# Comparaison de l'improvisation obtenue avec la (les) mélodie(s) originale(s)"
			
			./ModeleMarkov/bin/comparer.exe $modelisation $modelisation_impro $comparaison > $redirection_sortie
			
			echo "$comparaison" >> $fichier_historique
		done
	fi
	
	# Fin du script
	
	echo -e ""
	
	echo "" >> $fichier_historique
	
else
	echo -e "# Utilisation : ./launch.sh <fichiers-musicaux> [<options>]"

	echo -e "# Formats musicaux acceptés :"
	echo -e "\tMIDI\t\t.mid"
	echo -e "\tMusicXML\t.xml"
	
	echo -e "# Options :"
	echo -e "\t--impro-aleatoire\tGénérer, à partir de la partition, une mélodie totalement aléatoire"
	echo -e "\t--impro-contraintes\tGénérer, à partir de la partition, une mélodie aléatoire sous contraintes"
	echo -e "\t--impro-markov\t\tGénérer, à partir de la partition, une mélodie basée uniquement sur le modèle de Markov"
	echo -e "\t--nombre-notes <NB>\tIndiquer le nombre de notes à générer par improvisation, avec NB le nombre de notes à générer"
	echo -e "\t--notraces\t\tMasquer la sortie lors de l'utilisation des programmes"
	
	exit 1
fi

exit 0

