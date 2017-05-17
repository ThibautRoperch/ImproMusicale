make clean

make

make convert in=partitions/ActorPreludeSample.xml out=test.xml

make launch in=test.xml



clear ; make clean ; make ; make epurer in=partitions/ActorPreludeSample.xml out=test.xml
clear ; make clean ; make ; make modeliser in=melodies/*
clear ; make clean ; make ; make verifier in=melodies/* genere=aaa.xml

