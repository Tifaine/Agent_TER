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