# BonkHG

Deze handleiding beschrijft de wijzigingen op de branch `bonkhg` van [Bonk7/hg-engine-bonk7](https://github.com/Bonk7/hg-engine-bonk7/tree/bonkhg). Het is een HeartGold-enhancement met aangepaste wilde encounters en trainerteams in Johto en Kanto, geïnspireerd op Storm Silver. Er zijn geen levelcaps.

## Lokaal bouwen op Linux of WSL

Als je bestaande clone op `bonkhg` staat en `origin` naar de fork wijst, volstaat na iedere update:

```sh
git pull --ff-only
make -j3
```

Open na **Done. See output test.nds** het bestand `test.nds` uit deze repository. De build maakt of herstelt zelf de lokale Python-omgeving en haalt zo nodig de nitrogfx-submodule op. Je hoeft geen venv te activeren, geen losse Python-pakketten te installeren en geen gegenereerde bestanden uit de cloud te kopiëren.

### Eenmalige installatie

- Gebruik een Git-clone. Controleer met `git remote -v` of `origin` naar de fork wijst en met `git branch --show-current` of je op `bonkhg` staat. Ga zo nodig naar die branch met `git switch bonkhg`.
- Plaats je eigen schone Amerikaanse HeartGold-ROM met gamecode `IPKE` als `rom.nds` in de hoofdmap. Bewaar je bestaande saves en de originele ROM apart. Git levert de basis-ROM niet mee.
- Op Debian, Ubuntu en Ubuntu onder WSL installeer je eenmalig de systeempakketten. Installeer ook een actuele stabiele Rust-toolchain via [rustup](https://rustup.rs/) en zorg dat `cargo --version` werkt. De eerste build downloadt tools en Python-pakketten en heeft daarvoor internet nodig.

```sh
sudo apt update
sudo apt install build-essential git libpng-dev cmake pkg-config python3 python3-venv gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi curl
```

Voor andere systemen staan de installatie-instructies in de [upstream README](../README.md). Als je bij een bestaande clone toch een oude build of foutmelding krijgt, controleer eerst de branch en de naam van het ROM-bestand. `make clean` gevolgd door `make -j3` maakt de afgeleide ROM-data opnieuw. De bron-ROM en emulator-saves vallen buiten deze opruimactie. Bij een fout in de build is de eerste foutmelding meestal het nuttigst. `Error 2` onderaan meldt alleen dat een stap eerder is mislukt.

## Praktische testroute

Gebruik DeSmuME of melonDS met twee emulatorprofielen of afzonderlijke savebestanden. Start één **Normal game** voor verhaal en regressies en één **Debug menu** voor snelle controles. Sla in het spel zelf op vóór een belangrijke test. Laad bij een andere ROM-build de normale in-game save via **Continue**. Emulator-savestates bevatten ook code en geheugen van de oude ROM.

### 1. Nieuwe save en eerste badge

| Test | Zo controleer je het |
| --- | --- |
| Debugopt-in | Kies in de slaapkamer de gewenste modus. Open buiten het spelermenu met **X**, druk op **SELECT** en vergelijk beide saves. Alleen op de debugsave verschijnt het menu. |
| Zoekfuncties | Zoek op een deel van een itemnaam, zoals `SHINY`, en op een deel van een soortnaam, zoals `PIKA`. Geef één item en één Pokémon en controleer tas, team en gekozen level. Voeg een badge toe en controleer de Trainer Card. Badge-toekenning verandert geen verhaalvlaggen. |
| Warps en snelle winst | Kies **Warp to town**. Controleer aankomst, wandelen en in- en uitlopen van een gebouw, bijvoorbeeld bij Violet en daarna in Kanto. Tijdens de gevechtskeuze geeft **L+A** op de debugsave een directe overwinning. Test op de normale save ook dat deze combinatie geen overwinning geeft. |
| Wilde encounters en trainers | Speel Route 30, Route 31 en Sprout Tower. Vergelijk soorten, levels en teams met de Pokédex-gids en de trainerdata. Herhaal halverwege Johto, rond de League en in Kanto. Test gras, water, hengel en verschillende dagdelen waar die relevant zijn. |
| Verhaal-Mega | Spreek Elm bij de ingang van Sprout Tower, versla Sage Li en meld je terug. Controleer **Mega Quest Log**, **Mega Ring** en **Beedrillite**. Spreek Falkner vóór en na Elms opdracht. Laat Beedrill de steen vasthouden en activeer Mega Evolution in het aanvalsmenu. |

### 2. Pokémon Center, route en gevecht

| Feature | Test en verwacht resultaat |
| --- | --- |
| Center-diensten | Gebruik in verschillende Centers de medewerker voor teamdiensten, Move Reminder, Exp. Share, eieren, Mega-onderzoek en gevechtsinstellingen. De verpleegster geneest en de Shiny Trial heeft een eigen NPC. Controleer ook de League-lobby. |
| Training en statbewerking | Train een Pokémon onder het getoonde doellevel. Gebruik Move Reminder om eventueel gemiste zetten te leren. Verplaats de selectie met **omhoog/omlaag** en schuif met **links/rechts**. Gebruik **L/R** voor grotere stappen, **X** om de gekozen stat te wissen en **Y** om alle waarden te wissen. Met **A** pas je de wijzigingen toe en met **B** annuleer je ze. Controleer 252 per stat en 510 totaal. Na de Champion kun je hetzelfde doen voor IV's, tot 31 per stat. |
| Move Reminder en Exp. Share | Laat een vergeten zet gratis terugleren. Zet team-Exp. Share aan/uit, win een gevecht en vergelijk de ervaring van een Pokémon die niet heeft meegevochten. Sla op en laad de instelling opnieuw. |
| Egg Moves | Kies **Egg Moves** bij de medewerker in een Pokémon Center. Selecteer een Pokémon en bekijk type, categorie, kracht, nauwkeurigheid, PP en beschrijving met de D-pad. Een aangeleerde zet kost ₽500. Annuleren kost niets. Een Pokémon met vier zetten gebruikt het normale vervangscherm. Controleer ook annuleren en een Pokémon zonder nieuwe Egg Moves. De Training-pagina in de Pokédex verwijst naar de medewerker. |
| Mega Stone Box | Zodra je een Mega Ring of een Mega Stone bezit, verschijnt de **Mega Stone Box** bij **Key Items** als je de tas opent. Kies **Use** om je stenen te bekijken of aan een Pokémon te geven. Met **B** ga je terug naar de tas. **Mega** en **SELECT** blijven beschikbaar als snelkoppeling. |
| Box Link | Haal het sleutelitem bij de medewerker. Open buiten een Center de box vanuit de tas. Sla Story, Catching en Training op, verplaats teamleden naar de box en laad een preset. Probeer ook een preset met een ontbrekend teamlid of een teamlid met Mail. Een mislukte wissel laat het team intact. |
| Hergebruik van Repel | Gebruik een Repel en loop tot het effect afloopt. Bevestig het aanbod om hetzelfde type opnieuw te gebruiken en controleer de voorraad. |
| Field HMs en rematches | Probeer een geschikte veldactie met de juiste HM en badge, zonder de zet aan een teamlid te leren. Bel vervolgens een rematchcontact buiten het oorspronkelijke tijdvenster. De gewone voortgangsvoorwaarden gelden nog. |
| Nature Mints en abilities | Controleer een gewone Poké Mart bij 3 badges voor Mints en Ability Capsule en bij 8 badges voor Ability Patch. Gebruik ze op een geschikte Pokémon en controleer stat- of abilitywijziging. |
| Fokken | Haal het eerste ei op bij de Day Care op Route 34. Laat daarna opnieuw een ei ontstaan en haal het op bij de Center-medewerker. Controleer ook het kortere uitbroeden en het gesprek bij een vol team. |
| Gevechtsinstellingen | Open **Options** in het spelersmenu. Kies bij **Battle Speed** met links en rechts of door te tikken voor 1× (standaard), 2×, 3× of 4×. De groene waarde is actief. Alle instellingen gelden direct en worden meegenomen wanneer je het spel opslaat. Gewone trainers en wilde gevechten gebruiken de gekozen snelheid. Gym Leaders, rivalen, Rocket-bazen, de Elite Four, de Champion, Red, Frontier-gevechten en de eerste fase van legendary trials draaien op 1×. De muziek houdt haar eigen tempo. **Battle pace** verkort apart de tekstpauzes en slaat enkele routineanimaties over bij gewone gevechten. Schakel **Type hints** onafhankelijk om. Vergelijk routineanimaties en tekstpauzes. Controleer bij aanvallen groen voor **Super eff.**, oranje voor **Resisted** en rood voor **No effect**. Bij verschillende uitkomsten in een dubbelgevecht kan **Varies** verschijnen. |
| Vanginformatie | Vecht tegen een al gevangen wilde soort. Controleer de melding en bekijk de Quick Ball in de gevechtstas op beurt één en daarna. De omschrijving geeft aan of de situatiebonus actief is. |
| Vangstbeheer | Vang een gewone wilde Pokémon. Na de bijnaamvraag verschijnen **Summary**, **Send to Box** en **Put in team**. Probeer zowel vrije als volle teamplekken. Kies bij zes leden wie naar de box gaat en controleer beide Pokémon. Test Mail of Seal Capsule op het te vervangen teamlid. |

### 3. Pokédex en evoluties

Houd de combinatie ingedrukt terwijl je een soortentry opent. Blader in de gidsen met **L/R**. De gewone entry toont de bediening.

| Invoer | Weergave |
| --- | --- |
| SELECT | Evolutievoorwaarden, inclusief alternatieven voor ruilen |
| SELECT+X | Gedetailleerde vindplaatsen voor deze soort |
| SELECT+Y | Soortprofiel met stats, abilities en learnsetinformatie |
| SELECT+X+Y | Encountergids van de huidige route, met methode, levels en gevangen status |
| SELECT+START | Training met EV-opbrengst, level-up, TM/HM, egg moves en tutor-moves met locatie of vereisten |
| SELECT+L+R | Oorspronkelijke soortbeschrijving |

Controleer de gewone area-markers en de gedetailleerde gids met een soort die je net op die route hebt gevangen. Test een single-player ruilevolutie, bijvoorbeeld Kadabra met een Linking Cord. Gebruik voor item- of soortspecifieke alternatieven de evolutiegids.

### 4. Mega-journal en teams

- Open het log vanuit **Key Items**. Controleer het actieve tabblad, de groene Completed-lijst, paginering en de standaard alfabetische volgorde. Druk op **R** voor sorteren op naam, gebied of beloning. De blauwe balk toont het aandeel afgeronde quests en het percentage.
- Open met **A** een verborgen quest voor de locatiehint en een ontdekte of voltooide quest voor het doel, de beloning en het itemicoon. Met **B** keer je terug naar de lijst.
- Test één opdracht per type bij de eigen questgever. Ruil een Oran Berry voor Heracronite in Ilex Forest, vecht tegen de leerling met Mega Scizor bij Kurt thuis of open het Bellsprout-onderzoek in het ruilhuis van Violet City. De reddingsoefening tegen Mega Feraligatr begint in het zuidwestelijke huis van Cherrygrove. De 67 locaties staan in de [lijst van questgevers](../data/mega_quest_locations.tsv). Test ook weigeren, ontbreken van het vereiste en opnieuw openen na voltooiing.
- Er zijn 91 stenen verdeeld over Elms verhaalbeloning, 23 eenmalige vondsten en 67 sidequests. Het log telt 68 quests inclusief Elm. Voor de volledige locaties en opdrachten zie [vindplaatsen](../data/mega_stone_pickups.tsv) en [sidequests](../data/mega_stone_quests.tsv). De drie vroege vondsten staan op Sprout Tower 3F, langs Route 32 en in Union Cave B1F.
- Voor 25 sterkere Mega Stones staan er langere proeven in het logboek. Johto-proeven vragen vier tot acht Johto-badges. De geselecteerde postgameproeven openen na de Champion. Elk van deze gevechten heeft vier of vijf Pokémon en eindigt met de bijbehorende Mega. Bekijk het logboek voor de vereisten en test dezelfde opdracht vóór en na het behalen ervan.
- Onder **Milestone rewards** haal je na iedere vijf quests drie Rare Candies of drie Heart Scales op, afwisselend. Bij iedere tien komt daar één Shiny Ball bovenop. Bewaar een save bij 4 en 9 quests, voltooi er nog één, haal het pakket op en controleer dat opnieuw ophalen niets dubbel geeft.
- Test één boss en één gewone trainer zonder **L+A** tot minstens één aanval is gekozen. Er zijn 66 aangepaste bossvarianten en 40 gewone trainerteams met een Mega. William in het National Park heeft Mega Raichu, Union Cave B1F heeft Mega Kangaskhan en Wade gebruikt Mega Beedrill in een latere rematch. Rayquaza gebruikt Dragon Ascent voor Mega Evolution.

### 5. Champion en postgame

| Feature | Test en verwacht resultaat |
| --- | --- |
| Champion-ontgrendelingen | Gebruik een save waarop je de League echt hebt gewonnen. Controleer IV-bewerking, de Shiny Trial en de onderzoeksopties in de Ruins of Alph. Een via debug toegekende badge vervangt de Champion-overwinning niet. |
| Shiny Trial | Praat met de aparte NPC. Er zijn drie gevechten zonder automatisch herstel tussen de rondes. Eerst Morty of Jasmine, dan Koga of Karen en als laatste Lance of Blue, met hun rematchteams. Drie overwinningen geven één Shiny Ball. Je kunt de trial herhalen. |
| Shiny Ball | Test een geldige worp op een gewone wilde Pokémon. De vangst slaagt gegarandeerd en de gevangen Pokémon is shiny. Bekijk dit in Summary en na opslaan en herladen. |
| Statische legendaries | Versla eerst fase 1, controleer dat je terugkeert naar het veld en sla op vóór je zelf fase 2 start. Test soft reset, vluchten en opnieuw uitdagen. Beide fasen gebruiken 31 IVs per stat en een soortspecifieke verdeling van 252/252/6 EVs. Fase 2 gebruikt de gewone shinykans. |
| Ruins of Alph en Arceus | Na de Champion geeft de onderzoeker toegang tot Regirock, Regice en Registeel. De drie vangsten openen Regigigas. Arceus opent wanneer alle andere reguliere soorten in de uitgebreide nationale Pokédex gevangen zijn. Test de afwijzing met een ontbrekende soort vóór je een volledige save gebruikt. De beschikbaarheid van alle soorten in één normale playthrough is nog niet volledig geverifieerd. |

De statische proeven omvatten Articuno, Zapdos, Moltres, Mewtwo, Suicune, Lugia, Ho-Oh, Latias, Latios, Kyogre, Groudon, Rayquaza, de drie Regis, Regigigas en Arceus.

## Regresstests en bronbestanden

Controleer ook **Continue**, Center in/uit in Violet City, een gewone route na een warp, een verlaten menu en meerdere Pokédex- en questlogpagina's. Roze dialoogpijltjes mogen na het sluiten van een menu niet blijven hangen. Controleer bij een bug de commit, emulatorversie, locatie en handelingen en bewaar zo mogelijk een normale in-game save van vlak ervoor.

| Onderdeel | Bron |
| --- | --- |
| Wilde encounters en trainerteams | [Encounters.c](../data/Encounters.c), [Trainers.c](../data/Trainers.c) |
| Evolutievoorwaarden | [Evolutions.c](../data/Evolutions.c) |
| Mega-combinaties en quests | [mega_stones.inc](../include/mega_stones.inc), [build_mega_research.py](../scripts/build_mega_research.py), [questlogscript](../data/scr_seq/mega_quest_log.inc) |
| Gevechtsregressies | [Battle test README](../data/battle_tests/README.md). Bouw deze in een aparte clone met `make AUTO_TEST=Y -j3` en start op Linux met `SDL_VIDEODRIVER=dummy scripts/run_tests.sh -j 3`. |
| Resultaten per functie | [BonkHG regressiecontrole](BONKHG_QA.md) geeft per functie de uitgevoerde controles en de open testgevallen. |
| Eerdere build- en emulatorchecks | [QOL_PROGRESS.md](../QOL_PROGRESS.md) beschrijft de ontwikkelgeschiedenis, inclusief tijdelijke implementaties die later zijn vervangen. |


## Operation Aftershock

Na de Pokémon League start je dit Kanto-verhaal bij de agent in de lobby van Silph Co. in Saffron City. De agent stuurt je naar de Lavender Radio Station. Versla daar de infiltrant met Mega Houndoom en volg zijn aanwijzing naar de westelijke trap van Seafoam Islands B3F.

Giovanni vecht met zes Pokémon op level 76–80, waaronder Mega Kangaskhan en Heatran. Keer na je overwinning terug naar Silph Co. voor het Silph-pakket met 1 Shiny Ball, 1 Ability Patch, 5 Rare Candies en 3 Heart Scales. Een nederlaag laat je het gevecht opnieuw proberen. Als de tas vol is, kun je de resterende items later ophalen. Heb je de Ability Patch al in een oudere versie opgehaald, dan kun je de andere items alsnog krijgen.

Als een bestaande build bij de gevechtsintro blijft hangen, gebruik dan `make -W rom.nds -j3` om opnieuw te beginnen vanaf de schone basis-ROM.

## Vroege ruilen in Johto

| Locatie | Geef | Krijg | Waar je de gevraagde Pokémon vindt |
| --- | --- | --- | --- |
| Cherrygrove, huis ten zuidwesten van het Center | Sentret | Surskit | Route 29, overdag of in de ochtend |
| Violet City, noordwestelijk huis | Kricketot | Numel | Route 31, in de ochtend of overdag |
| Azalea Town, huis van Kurt | Sandshrew | Snubbull | Union Cave 1F |
| Goldenrod City, huis van de Friendship Checker | Drowzee | Swinub | Route 34 |

Elke ruil kan één keer. Je krijgt een basisevolutie op het level van de Pokémon die je inlevert, met gemiddelde IV’s, de gewone level-up-moves en zonder vastgehouden bonusitem.
