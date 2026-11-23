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

De Shiny Trial staat na de Champion in het verpleegstersmenu. Drie opeenvolgende rematches zonder herstel tussen de rondes leveren één Shiny Ball op. Elke ronde kiest uit twee teams. Er is geen limiet op deelname. De Shiny Ball garandeert een geldige vangst en gebruikt in de worp en de opslag het bestaande Poké Ball-model. De gevechtstas toont een eigen icoon en beschrijving.

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
