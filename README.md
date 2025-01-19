# 🌐 Projet Webserv

## 🔎 Description du projet

Le projet Webserv est une étape clé pour comprendre et appliquer les bases du protocole HTTP (Hypertext Transfer Protocol). L'objectif principal de ce projet est de développer un serveur HTTP entièrement fonctionnel en C++ 98, capable de gérer les requêtes et réponses définies par le protocole HTTP/1.1. Ce serveur devra servir du contenu statique, gérer les méthodes HTTP et exécuter des scripts CGI, tout en garantissant une compatibilité avec les navigateurs web modernes.

---

## 🎯 Objectifs Secondaires

Pour atteindre l'objectif principal, les objectifs secondaires suivants sont indispensables :

1. **Implémentation des Méthodes HTTP :** Assurer la gestion correcte des méthodes `GET`, `POST` et `DELETE`.
2. **Architecture Serveur Non-Bloquante :** Développer un serveur non-bloquant basé sur des événements en utilisant `poll()` ou un mécanisme équivalent.
3. **Support des Fichiers de Configuration :** Créer un système de configuration robuste et flexible inspiré de NGINX.
4. **Gestion des Erreurs :** Fournir une gestion complète des erreurs, y compris des pages d'erreur par défaut.
5. **Communication Client-Serveur :** Garantir une interaction correcte entre le serveur et les clients (navigateurs) avec des codes de statut HTTP précis.
6. **Téléversement de Fichiers :** Implémenter une fonctionnalité permettant aux clients de téléverser des fichiers sur le serveur.
7. **Tests de Résistance :** Assurer la résilience et la haute disponibilité du serveur sous des charges élevées.
8. **Exécution de CGI :** Permettre l'exécution de scripts CGI pour prendre en charge la génération de contenu dynamique.
9. **Écoute Multi-Ports :** Autoriser le serveur à écouter simultanément sur plusieurs ports.

---

## 🎓️ Objectifs Pédagogiques

Ce projet vise à aider les membres de l'équipe à :

- Comprendre les concepts fondamentaux du protocole HTTP/1.1.
- Travailler avec la programmation bas-niveau des sockets et descripteurs de fichiers.
- Développer des compétences en programmation événementielle et en E/S non-bloquantes.
- Améliorer les techniques de débogage, notamment dans des environnements multi-threadés et basés sur des événements.
- Favoriser le travail en équipe et les compétences en gestion de projet grâce à la collaboration et à la répartition des tâches.
- Découvrir les standards RFC et les outils de test pratiques tels que `telnet` et NGINX pour valider le serveur.

---

## ⚙️ Stratégie de Développement (CI/CD)

Pour garantir un processus de développement professionnel et fluide, la stratégie CI/CD suivante sera mise en place :

1. **Intégration Continue :**

   - Compilation automatique du code et exécution automatisée des tests à chaque push sur les branches `main` et `develop`.
   - Vérification des fuites mémoire avec Valgrind.
   - Analyse statique pour garantir le respect de la norme C++ 98 et des bonnes pratiques de codage.

2. **Déploiement Continu :**

   - Déploiement des versions de test dans un environnement de staging pour des tests manuels et automatisés avec des navigateurs.
   - Ajout progressif des fonctionnalités via des branches dédiées, fusionnées uniquement après validation de tous les contrôles CI.

3. **Mises à Jour de la Documentation :**

   - Mise à jour d'une documentation pour les fonctionnalités, configurations et tests.
   - Mise à jour régulière de cette section au fur et à mesure de l'évolution de la stratégie pendant le cycle de vie du projet.

---

## 🤝 Contribution

Pour assurer une collaboration efficace :

- Respectez la convention de nommage établie en amont du projet.
- Utilisez les pull requests pour les revues de code avant toute fusion dans les branches `main` et `develop`.
- Communiquez sur les progrès lors de points d'équipe réguliers.

---

## 📊 Statistiques du Projet

![Nombre de commits](https://img.shields.io/github/commit-activity/m/Gregory-Marquiset/mcg_webserv)
![Langages principaux](https://img.shields.io/github/languages/top/Gregory-Marquiset/mcg_webserv)
![Pourcentage des fichiers](https://img.shields.io/github/languages/code-size/Gregory-Marquiset/mcg_webserv)

---

## 📞 Contact

Pour toute question ou suggestion, n'hésitez pas à nous contacter via le dépôt GitHub du projet nos github personnel [**Marie Langlois**](https://github.com/mlanglois26) (malanglo), [**Charles-Louis Dutel**](https://github.com/WPMad51) (cdutel), [Gregory Marquiset](https://github.com/Gregory-Marquiset) (gmarquis).

Ou si vous êtes actuellement ou anciennement étudiant à 42 via l'intra.

<p align="center">
  <a href="https://profile.intra.42.fr/users/malanglo">
    <img src="https://badge.mediaplus.ma/greenbinary/malanglo?1337Badge=off&UM6P=off" alt="Marie's 42 Badge" />
  </a>
  <a href="https://profile.intra.42.fr/users/cdutel">
    <img src="https://badge.mediaplus.ma/greenbinary/cdutel?1337Badge=off&UM6P=off" alt="Charles's 42 Badge" />
  </a>
  <a href="https://profile.intra.42.fr/users/gmarquis">
    <img src="https://badge.mediaplus.ma/greenbinary/gmarquis?1337Badge=off&UM6P=off" alt="Greg's 42 Badge" />
  </a>
</p>
