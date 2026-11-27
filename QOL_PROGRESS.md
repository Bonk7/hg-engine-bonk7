# QoL voortgang

Werk op `bonkhg`. Maak één commit per feature, push elke commit meteen naar `origin/bonkhg` en verifieer de remote SHA. Bouw in de cloud met `source /workspace/scratch/bonkhg-tools/env.sh` en `make -j3`. Deel geen ROM of testsave via Git. Voeg geen levelcaps toe.

| Feature | Status |
| --- | --- |
| Services in alle Pokémon Centers | Cloudbuild geslaagd. Een aparte medewerker biedt de diensten aan in alle 24 Center-lobby’s en de Pokémon League. De verpleegster gebruikt haar gewone herstelgesprek. |
| Pokédex per route | Cloudbuild geslaagd. SELECT+X+Y bij het openen van een entry toont de huidige route met vangmethodes, levels en gevangen status. L of R bladert. |
| Nature Mints | Cloudbuild geslaagd. Alle 21 Mints zijn vanaf drie badges verkrijgbaar bij de gewone Poké Mart. |
| Ability Capsule en Patch | Cloudbuild geslaagd. Capsule vanaf drie badges, Patch vanaf acht badges bij de gewone Poké Mart. |
| Vanginformatie in gevechten | Cloudbuild geslaagd. Gewone wilde gevechten melden wanneer je de soort al gevangen hebt. De omschrijving van situationele Poké Balls in de gevechtstas toont vóór bevestiging of de vangkansbonus geldt. De vangberekening en de omschrijving delen dezelfde formule. |
| Fokken | Cloudbuild geslaagd. Uitbroeden gaat circa driemaal sneller. Na het eerste ei op Route 34 kun je wachtende eieren in Pokémon Centers ophalen. |
| Optionele korte gevechtsanimaties | Cloudbuild geslaagd. Schakelbare korte pauzes en overslaan van reguliere weer-, stat- en herstelanimaties. |

De opstartregressie van de routegids is hersteld door de savefunctie van hg-engine te gebruiken. De herstelde ROM bereikte het titelscherm en laadde de bestaande testsave in Cherrygrove in DeSmuME.

Runtimechecks in DeSmuME op de cloudbuild. Cherrygrove toont de gedeelde Center-services. De gids toonde de eerste Route 30-encounter. In een wild gevecht toonde de Quick Ball eerst een actieve bonus en na één beurt geen bonus. Een worp eindigde in de normale Pokédex-registratie.

De standaard Pokédex-entry toont nu de bediening van de gidsen. SELECT opent evoluties, SELECT+X vindplaatsen, SELECT+Y het soortprofiel en SELECT+X+Y de huidige route. SELECT+L+R toont de oorspronkelijke soortbeschrijving. De hulppagina is in DeSmuME zichtbaar gecontroleerd.

De Shiny Trial heeft een eigen NPC in alle 25 Pokémon Center-lobby’s, inclusief de Pokémon League. De NPC biedt de uitdaging aan na de Champion. In DeSmuME zijn beide gesprekken gecontroleerd in Cherrygrove. Drie opeenvolgende rematches zonder herstel tussen de rondes leveren één Shiny Ball op. Elke ronde kiest uit twee teams. Er is geen limiet op deelname. De Shiny Ball garandeert een geldige vangst en gebruikt in de worp en de opslag het bestaande Poké Ball-model. De gevechtstas toont een eigen icoon en beschrijving.

De cloudbuild slaagde. In DeSmuME heb ik de uitleg en de eerste rematch van de Shiny Trial gezien met een testsave waarop de Champion-vlag was gezet. Ik heb met een testexemplaar van de Shiny Ball een Caterpie gevangen en de shinywaarde van het opgeslagen exemplaar gecontroleerd. De volledige reeks van drie overwinningen is niet doorgespeeld.

Bij New Game verschijnt in de slaapkamer eenmaal de keuze Normal game of Debug menu. Op een debugsave open je het spelermenu met X en druk je daarna op SELECT voor het debugmenu. Kies een Johto- of Kanto-badge, een item of een Pokémon op naam. De item- en Pokémonlijsten hebben paginering en een zoekoptie met het touchscreen-toetsenbord. Zoeken werkt ook op een deel van de naam. Na selectie kies je de hoeveelheid of het level. De badge-optie verandert de badge op de Trainer Card. Verhaalvlaggen blijven intact.

Cloudbuild geslaagd. In DeSmuME verschenen beide keuzes bij een echte nieuwe game. De normale en debugkeuze zijn gecontroleerd op de opgeslagen savevlag. Op een testfixture werkte het toevoegen van de Zephyr Badge, een Poké Ball en een Pikachu.

De debugtoegang is verplaatst van de verpleegster naar het spelermenu. In DeSmuME heb ik de SELECT-sneltoets op een normale save en debugsave gecontroleerd en de debugtools gesloten en opnieuw geopend op straat.

Legendaries, eerste groep. Na de Champion toont de onderzoeker in het Ruins of Alph Research Center de nieuwe signalen. Regirock, Regice en Registeel zijn vaste gevechten op level 66. De drie vangsten openen Regigigas op level 76. Arceus verschijnt op level 100 zodra alle andere reguliere soorten in de uitgebreide nationale Pokédex gevangen zijn. De teller slaat Arceus zelf, Egg en Bad Egg over. Gevluchte en verslagen legendaries kunnen opnieuw worden uitgedaagd.

Cloudbuild, opstartcontrole en de verwijzing naar het oorspronkelijke onderzoeksgesprek zijn getest. De vangsten zijn nog niet in de emulator gespeeld. De overige legendaries en de beschikbaarheid van alle soorten voor de Arceus-voorwaarde staan nog open.

De medewerker staat in de gewone lobby's rechts van de ontvangstmat en heeft een eigen plek bij de Pokémon League. De zone-events bevatten precies één nieuwe medewerker per lobby. In DeSmuME gecontroleerd dat de medewerker in Cherrygrove het dienstenmenu opent en de verpleegster haar gewone herstelgesprek gebruikt.

Bij reguliere wilde vangsten vervalt de Pokédex-preview. Na de bijnaamvraag en terugkeer naar het veld kun je het overzicht bekijken, de vangst in de box zetten of haar aan je team toevoegen. Bij een vol team kies je wie plaatsmaakt. Een teamlid met Mail of een Seal Capsule kan pas naar de box als je die hebt verwijderd. Speciale vangstmodi zoals Safari en Bug-Catching Contest behouden hun eigen afhandeling.

Cloudbuild geslaagd. In DeSmuME is een wilde Oddish gevangen met een vol team. Alle drie de menuopties zijn gecontroleerd. Bij de wissel op de derde teamplek is gecontroleerd dat Oddish daar stond, het gekozen teamlid in de box stond en de overige vijf teamplekken dezelfde Pokémon behielden.

De cloudbuild met de doorzoekbare debuglijsten slaagde. In DeSmuME is vanuit het spelermenu gezocht op SHINY en PIKA. De zoekresultaten leverden een Shiny Ball in de tas en een Pikachu in het team op.

Op een debugsave opent Warp to town een lijst van alle steden en dorpen in Johto en Kanto, inclusief Cinnabar Island en Indigo Plateau. Je kunt kiezen zonder Fly of een eerder bezoek. De warp gebruikt het bestaande aankomstpunt van Fly. In DeSmuME zijn de aankomstcoördinaten en het geladen veldbeeld voor alle 21 bestemmingen gecontroleerd.

Op een debugsave geeft L+A tijdens de gevechtskeuze direct een overwinning in reguliere wilde en trainergevechten. Het spel handelt het resultaat af via het gewone overwinningsscript. In DeSmuME gecontroleerd met een wilde Ledyba en Sage Nico in Sprout Tower. In hetzelfde wilde gevecht op een normale save opende de combinatie het normale aanvalsselectiemenu.

Pokédex Training heeft een eigen weergave. Houd SELECT+START ingedrukt wanneer je een soortentry opent. L/R bladert door EV-opbrengst, level-upmoves, beschikbare TM/HM-moves, egg moves en de tutors in Ilex Forest, de Battle Frontier en Blackthorn City. De Blackthorn-pagina’s vermelden waar maximale vriendschap nodig is. De cloudbuild en de gegenereerde tekstbank zijn gevalideerd. In DeSmuME toonde Cyndaquil zijn Training-pagina met EV-opbrengst.

Pokémon Box Link heeft drie vaste teampresets. Sla je huidige team op als Story team, Catching team of Training team en laad het later terug. Een ontbrekend teamlid, een geblokkeerde verplaatsing of te weinig boxruimte laat het huidige team intact. In DeSmuME is een preset opgeslagen en teruggeladen met een Pokémon uit de box. De uitgaande Pokémon is in de box teruggevonden en de inkomende Pokémon stond alleen in het team.

Type hints staan standaard aan. Het aanvalsmenu toont Super effective groen, Not very effective oranje en No effect rood onder de PP-regel. Verschillende uitkomsten voor twee mogelijke tegenstanders tonen Depends on target. De medewerker in het Pokémon Center heeft Battle settings > Type hints om de weergave aan of uit te zetten. De cloudbuild slaagde. In DeSmuME zijn de drie gekleurde uitkomsten, de gewone weergave met de optie uit en de schakelaar bij de medewerker gecontroleerd.

De 17 statische legendaries hebben twee gevechten. Versla de eerste vorm om de vangpoging vrij te spelen. Na de overwinning kun je opslaan en zelf de tweede poging starten. Beide gevechten gebruiken zes IV’s van 31 en een soortspecifieke verdeling van 252/252/6 EV’s. De tweede poging gebruikt de normale shinykans. Bij de onderzoeker in de Ruins of Alph kies je hetzelfde signaal opnieuw. Arceus vereist nog steeds een voltooide Pokédex.

De medewerker in ieder Pokémon Center geeft via Mega Evolution een gratis Mega Ring en verkoopt de 91 ondersteunde Mega Stones voor ₽3.000 per stuk. De winkel is vanaf het eerste Pokémon Center toegankelijk. Geef de steen aan de bijbehorende Pokémon en activeer Mega Evolution in het aanvalsmenu. Voor Rayquaza geldt Dragon Ascent. De cloudbuild slaagde. In DeSmuME zijn de Mega Ring en de aankoop van Venusaurite bij de medewerker in Cherrygrove gecontroleerd.

Mega Evolution is ingericht voor 66 teamvarianten, waaronder alle 16 Gym Leaders en hun rematches, de Elite Four, Lance, Red, de vier Rocket Executives, Giovanni, Eusine en de latere gevechten tegen Silver. Elk aangepast team bevat één Pokémon met een passende Mega Stone. De teamgroottes en levels zijn behouden. Voorbeelden zijn Mega Pidgeot bij Falkner, Mega Starmie bij Misty, Mega Houndoom bij Archer, Mega Kangaskhan bij Giovanni, Mega Charizard X bij Lance en Mega Charizard Y bij Red.

De cloudbuild met de aangepaste teams slaagde. In DeSmuME is tegen Koga bevestigd dat de AI zijn Beedrill automatisch Mega Evolueert en dat de speler via de knop in het aanvalsmenu zijn eigen Beedrill kan Mega Evolueren. Alle 66 teamvarianten zijn gecontroleerd op één geldige soort-steencombinatie.

Elm begint zijn Mega-onderzoek bij de ingang van Sprout Tower. Versla Sage Li en meld je terug bij Elm voor de Mega Ring en Beedrillite. Falkner verwijst je naar Elm zolang het onderzoek openstaat. Op een oudere save telt een eerdere overwinning op Li mee. De bestaande steenwinkel is voorlopig beschikbaar nadat Elm de proef heeft afgerond, totdat vindplaatsen en sidequests de stenen overnemen. De cloudbuild en in DeSmuME Elms begin en beloning met een gesimuleerde overwinning op Li zijn gecontroleerd, evenals Falkners verwijzing naar Elm.

Elm geeft bij het eerste gesprek het Mega Quest Log. Open het in de Key Items-pocket. De eerste lijst toont ontdekte opdrachten in blauw en verborgen opdrachten als To be discovered met een locatiehint. De Completed-lijst toont afgeronde opdrachten in groen. R opent sortering op opdrachtnaam, gebied of beloning. De actieve opdrachten in deze versie zijn Elms proef en Grove supplies in het Pokémon Center van Azalea Town. Voor Grove supplies kun je één Oran Berry ruilen voor Heracronite.

De eerste drie vindbare stenen zijn Pidgeotite op Sprout Tower 3F, Ampharosite in het Pokémon Center op Route 32 en Steelixite in Union Cave B1F. Pidgey, Mareep en Onix zijn al in Johto verkrijgbaar. Beedrillite, Heracronite en deze drie vondsten zijn uit de tijdelijke winkel gehaald. De resterende overworld-vondsten, sidequests en de volledige verdeling over Johto en Kanto staan nog open.

De cloudbuild slaagde. In DeSmuME zijn openen vanuit de tas, beide lijsttabbladen, de statuskleuren, R en sorteren op gebied gecontroleerd. Alle drie vindbare stenen zijn opgepakt. In Azalea is met één Oran Berry op de testsave gecontroleerd dat de ruil de bes verbruikt, Heracronite geeft en de questvlag op afgerond zet.

Het Mega Quest Log toont op beide tabbladen het aantal afgeronde quests en een vijfdelige voortgangsbalk voor de volgende beloning. Via Milestone rewards haal je voor elke vijf afgeronde quests één pakket op. De pakketten wisselen tussen drie Rare Candies en drie Heart Scales. Bij elke tien afgeronde quests zit er ook één Shiny Ball in het pakket. Het logboek onthoudt opgehaalde pakketten in de save en laat een pakket klaarstaan als de tas vol zit.

De 91 Mega Stones hebben elk een eigen bron. Beedrillite komt uit Elms verhaalquest. Er zijn 23 eenmalige vondsten en 67 sidequests, inclusief Grove supplies. De medewerker van het Pokémon Center biedt onder Mega Evolution > Local Mega research de lokale opdrachten aan. De 66 nieuwe opdrachten bestaan uit korte leveringen, Pokédex-onderzoek, kennisvragen en optionele gevechten met een Mega-team. De tijdelijke winkel is vervangen. De tabbladen van het Mega Quest Log hebben paginering voor alle 68 quests, inclusief Elms proef. De tabelbestanden in data leggen alle locaties en beloningen vast.

De build controleert dat alle 91 stenen precies één geregistreerde bron hebben. In DeSmuME zijn de 20 nieuwe vindplaatsen vanaf een aangrenzende tegel opgepakt. Ook de teller van 68 opdrachten, een quiz, een voorwerpenruil, een Pokédex-opdracht en een optioneel trainergevecht zijn gecontroleerd. Voor de Pokédex-opdracht en trainerwinst is een testfixture gebruikt.

Ook 40 gewone trainerteams in Johto en Kanto hebben elk één Mega Stone. De spreiding omvat onder meer rematches op Routes 31, 32, 34 en 38, het National Park, Union Cave, Mt. Mortar, de S.S. Aqua en enkele Gym-trainers. De teams behouden hun grootte en levels. De steen gaat naar een bestaand teamlid of een evolutie uit dezelfde lijn. Routes zonder passende kandidaat houden hun gewone partijen.

## Event Quest Log

De Event Quest Log opent vanuit Key Items en houdt actieve en afgeronde events bij op aparte tabbladen. De lijst toont ook waar je nog onontdekte quests kunt starten.

| Locatie | Opdracht en vervolg |
| --- | --- |
| Pewter Museum | Toon Geodude voor de Enigma Stone. Die opent het Latias-event bij Steven in het museum. |
| Kurt's huis in Azalea | Toon Mareep voor een shiny Pichu. Neem het mee naar de Ilex Shrine voor het Spiky-eared Pichu-event. |
| Oostelijk huis in Mahogany | Toon Noctowl na de Radio Tower voor Celebi. Neem Celebi mee naar de Ilex Shrine voor het oorspronkelijke Giovanni-event. |
| National Park | Toon Sunkern na de Radio Tower voor Shaymin. Neem Shaymin mee naar de bloemenwinkel in Goldenrod voor de Gracidea. |
| Ruins of Alph Research Center | Win de League, los de Kabuto- en Aerodactyl-puzzels op en toon Unown. Ga met alleen Unown in je team naar de ingang van de ruïnes om de Sinjoh-quest te vervolgen. Arceus blijft de beloning voor een complete Pokédex. |

De cloudbuild slaagde. De vijf NPC-gesprekken zijn in DeSmuME getest met voorbereide spelvoortgang. De volledige oorspronkelijke events zijn nog niet allemaal doorgespeeld.

## Speelbaar personage bij New Game

Kies in de slaapkamer tussen Ethan, Lyra, Lance, Misty, Jasmine en Palmer. De keuze wordt opgeslagen en bepaalt hoe je personage er te voet uitziet. Daarna volgt de keuze tussen Normal en Debug. Het verhaal, de Trainer Card, gevechten en speciale animaties zoals fietsen en surfen gebruiken de oorspronkelijke protagonist. Bestaande saves behouden hun oorspronkelijke uiterlijk.
