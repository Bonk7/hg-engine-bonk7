# QoL voortgang

Werk op `bonkhg`. Maak één commit per feature, push elke commit meteen naar `origin/bonkhg` en verifieer de remote SHA. Bouw in de cloud met `source /workspace/scratch/bonkhg-tools/env.sh` en `make -j3`. Deel geen ROM of testsave via Git. Voeg geen levelcaps toe.

| Feature | Status |
| --- | --- |
| Services in alle Pokémon Centers | Cloudbuild geslaagd. Alle Centers die het gedeelde verpleegstersscript gebruiken bieden de services via de verpleegster aan. |
| Pokédex per route | Cloudbuild geslaagd. SELECT+X+Y bij het openen van een entry toont de huidige route met vangmethodes, levels en gevangen status. L of R bladert. |
| Nature Mints | Cloudbuild geslaagd. Alle 21 Mints zijn vanaf drie badges verkrijgbaar bij de gewone Poké Mart. |
| Ability Capsule en Patch | Cloudbuild geslaagd. Capsule vanaf drie badges, Patch vanaf acht badges bij de gewone Poké Mart. |
| Vanginformatie in gevechten | Cloudbuild geslaagd. Gewone wilde gevechten melden wanneer je de soort al gevangen hebt. Quick, Repeat en Net Ball melden na selectie wanneer hun bonus niet geldt. Vooraf tonen en overige ballen staan nog open. |
| Fokken | Cloudbuild geslaagd. Uitbroeden gaat circa driemaal sneller. Na het eerste ei op Route 34 kun je wachtende eieren in Pokémon Centers ophalen. |
| Optionele korte gevechtsanimaties | Cloudbuild geslaagd. Schakelbare korte pauzes en overslaan van reguliere weer-, stat- en herstelanimaties. |
