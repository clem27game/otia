# Langage de Programmation Otia

Otia est un langage de programmation personnalisé conçu pour la simplicité et la facilité d'utilisation, permettant aux utilisateurs d'effectuer des opérations mathématiques, de gérer des fichiers, de créer des variables et des conditions avec une syntaxe claire.

## Fonctionnalités

- **Opérations Mathématiques** : Effectuez des opérations arithmétiques de base comme l'addition, la soustraction, la multiplication et la division, ainsi que des puissances, des racines carrées, des cubes et des carrés.
- **Gestion de Fichiers** : Créez, lisez et écrivez des fichiers texte facilement.
- **Sortie Console** : Affichez des messages dans la console en utilisant une syntaxe simple.
- **Gestion des Variables** : Définissez et manipulez des variables dans votre code.
- **Instructions Conditionnelles** : Implémentez une logique conditionnelle de base pour contrôler le flux du programme.

## Aperçu de la Syntaxe

### Opérations Mathématiques
Pour effectuer des calculs mathématiques, utilisez la syntaxe suivante :
```ot
Otia.mathematique {
add = 10 + 5
sub = 20 - 7
mul = 6 * 4
div = 100 / 20
}
```
Vous pouvez également calculer des puissances, des racines carrées, des cubes et des carrés :

```
Otia.mathematique {
pow = 2 ^ 3
sqrt = sqrt(16)
cube = cube(3)
square = square(4)
}
```
# Gestion de Fichiers
Créer un fichier :

```
Otia.create.fichier {
\mon_fichier.txt
}
```
# Écrire dans un fichier :

```
Otia.write.fichier {
\mon_fichier.txt('Bonjour du langage Otia !')
}
```
# Lire un fichier :

```
Otia.read.fichier {
\mon_fichier.txt
}
```
# Sortie Console
Affichez des messages dans la console :

```
Otia.console {
\Hello World
}
```
# Variables
Définir une variable :

```
Otia.variable {
myVar = 42
}
```
# Instructions Conditionnelles
Utilisez des conditions pour contrôler l'exécution du programme :

```
Otia.condition {
if myVar > 10 {
    Otia.console {\myVar est supérieur à 10}
}
}
```
# Installation
Pour commencer avec Otia, clonez le dépôt ici présent:

```
git clone <repository-url>
```
# Exécutez votre code Otia :

```
./otia/main otia-run <votre_fichier.ot>
```
# Exemple d'Utilisation
Vous pouvez créer un fichier appelé test.ot pour expérimenter avec le langage :

```
Otia.mathematique {
add = 10 + 5
}

Otia.console {
\Résultat: {add}
}
```
# Contribution

N'hésitez pas à contribuer en nous avertissant des problèmes ou en faisant des demandes de tirage pour améliorer les fonctionnalités d'Otia !