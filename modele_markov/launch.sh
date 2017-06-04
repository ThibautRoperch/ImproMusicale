#!/bin/bash



if [ $# -eq 1 ]
then
	fichier=$1
	
	echo -e "\n# Partition $fichier"
	
	# Détection du format de la partition
	
	if [[ $1 == *".mid" ]]
	then
		fichier=$(echo $fichier | sed -e 's/.mid$//')
		format="midi"
	elif [[ $1 == *".xml" ]]
	then
		fichier=$(echo $fichier | sed -e 's/.xml$//')
		format="musicxml"
	else
		echo -e "\n# Format musical non pris en charge"
		echo -e "# Formats acceptés :"
		echo -e "\tMIDI\t\t.mid"
		echo -e "\tMusicXML\t.xml"
		echo -e ""
		exit 0
	fi
	
	fichier=$(echo $fichier | awk -F / '{print $NF}')
	fichier="$fichier.xml"
	
	# Extraction de la mélodie
	
	echo -e "\n# Extraction de la mélodie dans test/melodies/$fichier"
	
	if [ "$format" == "midi" ]
	then
		./bin/epurer_midi.exe test/partitions/$fichier test/melodies/$fichier
	elif [ "$format" == "musicxml" ]
	then
		./bin/epurer_musicxml.exe test/partitions/$fichier test/melodies/$fichier
	fi
	
	# Modélisation des contraintes
	
	echo -e "\n# Modélisation des contraintes dans test/contraintes/$fichier"
	
	./bin/modeliser.exe test/melodies/$fichier test/modelisations/$fichier
	
	# Improvisation sur la mélodie
	
	echo -e "\n# Improvisation sur la mélodie dans test/improvisations/$fichier"
	
	./bin/rmg -o test/improvisations/$fichier -c test/modelisations/$fichier -n 20 -g 50
	
	# Vérification de la mélodie générée
	
	echo -e "\n# Vérification de l'improvisation par rapport à l'original"
	
	./bin/verifier_melodie_generee.exe test/melodies/$fichier test/improvisations/$fichier
	
	exit 1
fi

