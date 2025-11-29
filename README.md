> [!NOTE]
> [November 29, 2025]
> 
> A new beta version is available on https://github.com/SimonCROS/push_swap_tester/tree/v2, with performances up to **10x faster** 🚀 !

# complexity

## Build

Compile with the following command

`make [fr|en]`

## Usage

**./complexity** \[**-vh**\] \[**-s** <ins>seed</ins>\] \[**-f** <ins>push_swap</ins>\] \[**--sorted**\] <ins>numbers</ins> <ins>iterations</ins> \[<ins>goal</ins>\] \[<ins>checker</ins>\]

L'exécutable push_swap est cherché par défaut dans le répertoire courant et parent.

Les options suivantes sont disponibles :

**-v**, **--version**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Affiche la version du testeur

**-h**, **--help**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Affiche l'aide.

**-s** <ins>seed</ins>, **--seed**=<ins>seed</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Génère les nombres en fonction de la graine.

**-o** <ins>output</ins>, **--output**=<ins>output</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Spécifie un fichier de sortie pour les logs.

**-f** <ins>push_swap</ins>, **--file**=<ins>push_swap</ins>\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Utilise <ins>push_swap</ins> en tant qu'exécutable push_swap.

**--sorted**\
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Envoie uniquement des nombres triés au programme.

### Demo :

![Demo Animation for 100 elements](../assets/v1.6.0-100-100-checker.gif)
![Demo Animation for 500 elements](../assets/v1.6.0-500-100.gif)

---

Merci à [@mhouppin](https://github.com/mhouppin) pour l'inspiration visuelle du testeur.\
Thanks to [@42cursus-youkim](https://github.com/42cursus-youkim) for the translations.
