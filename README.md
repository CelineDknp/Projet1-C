# Projet1-C

## Tests de performances

Afin de compiler le programme, il suffit de lancer dans le terminal :

	$ make

Ensuite lancer les tests de performances en faisant :

	$ ./malloc $VAL

où $VAL est la taille que vous souhaitez essayer.

## Tests unitaires

Si vous souhaitez lancer les tests unitaires, il faut compiler avec :

	$ make tests

Ensuite, vous pouvez lancer les tests avec

	$ ./tests

## Après les tests

Après avoir utilisé ce programme, vous pouvez supprimer tous les fichiers temporaires au moyen de :

	$ make rm
