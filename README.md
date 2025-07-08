
# Langage de Programmation Otia

Otia est un langage de programmation conçu pour la simplicité et la facilité d'utilisation, offrant maintenant **60+ fonctionnalités** réparties dans les domaines des mathématiques, de la gestion de fichiers, des variables, des tableaux, de la console et du système.

## 🚀 Fonctionnalités Principales

### ✨ Mathématiques Avancées (20 fonctionnalités)
- **Opérations de base** : addition, soustraction, multiplication, division
- **Opérations avancées** : puissances, modulo, PGCD, PPCM
- **Fonctions mathématiques** : racine carrée, cube, carré, valeur absolue
- **Trigonométrie** : sinus, cosinus, tangente (en degrés)
- **Logarithmes** : logarithme naturel et base 10
- **Séquences** : factorielle, suite de Fibonacci
- **Géométrie** : calcul d'aire de triangle (formule de Héron)

### 📁 Gestion de Fichiers Complète (10 fonctionnalités)
- **Opérations de base** : créer, lire, écrire des fichiers
- **Opérations avancées** : copier, supprimer, vérifier l'existence
- **Informations** : taille du fichier, nombre de lignes
- **Modification** : ajouter du contenu à un fichier existant

### 🖥️ Console Enrichie (5 fonctionnalités)
- **Affichage simple** : messages basiques
- **Affichage coloré** : texte en rouge, vert, bleu, jaune
- **Contrôles** : effacer la console, émettre un bip sonore
- **Personnalisation** : définir le titre de la console

### 📊 Variables Typées (10 fonctionnalités)
- **Types supportés** : entiers, flottants, chaînes de caractères, booléens
- **Opérations** : création, affichage, incrémentation, décrémentation
- **Gestion avancée** : types automatiques, conversion de types

### 🔢 Tableaux Dynamiques (5 fonctionnalités)
- **Création** : tableaux de taille variable
- **Manipulation** : définir et lire des valeurs par index
- **Calculs** : somme de tous les éléments
- **Tri** : tri automatique des éléments

### ⚙️ Fonctions Système (5 fonctionnalités)
- **Temps** : affichage de l'heure et de la date actuelles
- **Aléatoire** : génération de nombres aléatoires dans une plage
- **Contrôle** : pause d'exécution, sortie du programme

### 🔄 Conditions Étendues (5 fonctionnalités)
- **Opérateurs** : ==, !=, >, <, >=, <=
- **Types mixtes** : comparaison entre variables et valeurs littérales

## 📖 Guide de Syntaxe Détaillé

### Mathématiques
```ot
Otia.mathematique {
add = 10 + 5              // Addition
sub = 20 - 7              // Soustraction
mul = 6 * 4               // Multiplication
div = 100 / 20            // Division
pow = 2 ^ 3               // Puissance
mod = 17 % 5              // Modulo
min = 15 5                // Minimum
max = 15 5                // Maximum
gcd = 48 18               // PGCD
lcm = 12 8                // PPCM
abs = -42                 // Valeur absolue
fact = 5                  // Factorielle
fib = 10                  // Fibonacci
sqrt = 16                 // Racine carrée
cube = 3                  // Cube
square = 4                // Carré
sin = 30                  // Sinus (degrés)
cos = 60                  // Cosinus (degrés)
tan = 45                  // Tangente (degrés)
log = 10                  // Logarithme naturel
log10 = 100               // Logarithme base 10
triangle_area = 3 4 5     // Aire triangle (Héron)
}
```

### Gestion de Fichiers
```ot
// Opérations de base
Otia.create.fichier {
\mon_fichier.txt
}

Otia.write.fichier {
\mon_fichier.txt('Contenu du fichier')
}

Otia.read.fichier {
\mon_fichier.txt
}

// Opérations avancées
Otia.file.copy {
\source.txt \destination.txt
}

Otia.file.delete {
\fichier_a_supprimer.txt
}

Otia.file.size {
\mon_fichier.txt
}

Otia.file.exists {
\mon_fichier.txt
}

Otia.file.append {
\mon_fichier.txt('Texte à ajouter')
}

Otia.file.lines {
\mon_fichier.txt
}
```

### Console Avancée
```ot
// Affichage simple
Otia.console {
\Message normal
}

// Affichage coloré
Otia.console.color {
\rouge Message en rouge
}

Otia.console.color {
\vert Message en vert
}

Otia.console.color {
\bleu Message en bleu
}

Otia.console.color {
\jaune Message en jaune
}

// Contrôles de console
Otia.console.clear {
}

Otia.console.beep {
}

Otia.console.title {
\Titre de ma fenêtre
}
```

### Variables Typées
```ot
// Création de variables
Otia.variable {
nombre_entier = 42
nombre_flottant = 3.14159
texte = 'Bonjour Otia'
}

// Affichage de variables
Otia.variable.show {
nombre_entier
}

// Manipulation de variables
Otia.variable.increment {
nombre_entier
}

Otia.variable.decrement {
nombre_entier
}
```

### Tableaux
```ot
// Création d'un tableau
Otia.array.create {
mon_tableau 5
}

// Définir des valeurs
Otia.array.set {
mon_tableau[0] = 10
}

Otia.array.set {
mon_tableau[1] = 30
}

// Lire des valeurs
Otia.array.get {
mon_tableau[0]
}

// Opérations sur tableaux
Otia.array.sum {
mon_tableau
}

Otia.array.sort {
mon_tableau
}
```

### Fonctions Système
```ot
// Informations temporelles
Otia.system.time {
}

Otia.system.date {
}

// Nombre aléatoire
Otia.system.random {
1 100
}

// Contrôle d'exécution
Otia.system.sleep {
3
}

Otia.system.exit {
0
}
```

### Conditions
```ot
Otia.variable {
a = 15
b = 10
}

Otia.condition {
if a > b           // Supérieur
}

Otia.condition {
if a == 15         // Égal
}

Otia.condition {
if a != b          // Différent
}

Otia.condition {
if a >= 15         // Supérieur ou égal
}

Otia.condition {
if b <= 20         // Inférieur ou égal
}
```

## 🛠️ Installation et Usage

### Compilation
```bash
gcc main.c -o otia -lm
```

### Exécution
```bash
./otia otia-run votre_fichier.ot
```

## 📝 Exemples Pratiques

### Calculatrice Scientifique
```ot
Otia.mathematique {
sin = 45
cos = 45
tan = 45
}

Otia.console {
\=== Calculatrice Trigonométrique ===
}
```

### Gestionnaire de Fichiers
```ot
Otia.create.fichier {
\donnees.txt
}

Otia.write.fichier {
\donnees.txt('Données importantes')
}

Otia.file.copy {
\donnees.txt \sauvegarde.txt
}

Otia.file.size {
\donnees.txt
}
```

### Générateur de Nombres
```ot
Otia.system.random {
1 100
}

Otia.system.random {
1 100
}

Otia.system.random {
1 100
}
```

## 🎯 Fonctionnalités Uniques d'Otia

1. **Syntaxe Cohérente** : Toutes les fonctionnalités utilisent la même structure `Otia.domaine.action { }`
2. **Types Automatiques** : Détection automatique des types de variables
3. **Gestion d'Erreurs** : Messages d'erreur clairs et informatifs
4. **Performance** : Implémentation en C pour une exécution rapide
5. **Extensibilité** : Architecture modulaire permettant l'ajout facile de nouvelles fonctionnalités

## 🤝 Contribution

N'hésitez pas à contribuer au développement d'Otia en :
- Signalant des bugs
- Proposant de nouvelles fonctionnalités
- Améliorant la documentation
- Créant des exemples d'utilisation

## 📄 Licence

Ce projet est open-source et libre d'utilisation pour tous vos projets personnels et commerciaux.

---

**Otia v2.0** - 60+ fonctionnalités pour une programmation simple et puissante ! 🚀
