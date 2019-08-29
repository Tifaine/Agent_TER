Programme serveur pour SMA

## Le récupérer:
```Shell
> git clone --recurse-submodules https://github.com/Tifaine/Agent_TER
```

## Compiler:
### Première foi:
```Shell
> ./Configure/configure
> make
```

### Après:
```Sehll
> make
```

## Utilisation:
Vous pouvez lancer le programme comme suit :
```Shell
> ./bin/exec
```

## Options:
Il existe plusieurs options disponibles :
```Shell
> --name (-n) NomDeLAgent
> --address (-a) AdresseIpDuServeur
> --nbSlot (-s) NombreDeSlotDisponibles
```

Pour ajouter des patterns, un fichier config.xml dans le dossier bin doit être présent.
Il est de la forme suivante : 
```XML
<?xml version="1.0" encoding="UTF-8" ?>
<nom>objet1</nom>
<pattern>
	<footprint target="1" type="1" value="75" ValidFrequency="5000" SamplingFrequency="500">		
	</footprint>
	<footprint target="1" type="2" value="80" ValidFrequency="5000" SamplingFrequency="1000">		
	</footprint>
</pattern>
```
Target : 1 pour l'activité CPU, 2 pour la mesure du courant electrique

Type : 1 pour la moyenne, 2 pour un pic

Value : Valeur maximale autorisé

ValidFrequency : Fréquence de vérification des agents Organizer

SamplingFrequency : Fréquence de demande des agents Watcher




Pour ajouter une fonction de monitoring : un exemple est présent dans le fichier main.c.
La fonction doit ajouter sa valeur dans l'empreinte liée aux patterns monitorés.