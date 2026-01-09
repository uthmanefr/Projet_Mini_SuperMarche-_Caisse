#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAILLE_TABLE 13

//---------------------STRUCTURES PRODUITS -------------
typedef struct Produit {
    int id;
    char nom[30];
    float prix;
    int quantite;
    struct Produit *suivant;
} Produit;

typedef struct Liste {
    Produit *tete;
    Produit *queue;
    int taille;
} Liste;

typedef struct {
    Produit *table[TAILLE_TABLE];
} TableHachage;

// --------------------STRUCTURES CLIENTS ----------------
typedef struct ClientNode {
    int id;
    char nom[30];
    float total;
    struct ClientNode *filsg;
    struct ClientNode *filsd;
} ClientNode;

typedef struct {
    ClientNode *racine;
} ArbreClients;

// -------------------STRUCTURES FILE D'ATTENTE --------------
typedef struct ClientFile {
    int id;
    char nom[30];
    float total;
    struct ClientFile *suivant;
} ClientFile;

typedef struct File {
    ClientFile *tete;
    ClientFile *queue;
    int nef;
} File;

// ==================== STRUCTURES PILE TRANSACTIONS ====================
typedef struct Transaction {
    int idClient;
    char nomClient[30];
    int idProduit;
    char nomProduit[30];
    int quantite;
    float total;
    char date[20];
    struct Transaction *suivant;
} Transaction;
typedef struct Pile {
    Transaction *tete;
    int nef;
} Pile;

// ==================== FONCTIONS PRODUITS ====================

int hachage(int id) {
    //on calcule l indice dans la table de hachage
    //on utilisont l id du produit
    //qu on a inserer
    return id % TAILLE_TABLE;
}

TableHachage* creerTableHachage() {
    //alloue la memeoire dynamiquement
    TableHachage* th = (TableHachage*)malloc(sizeof(TableHachage));

    if (th == NULL) {
        printf("Erreur d'allocation mémoire \n");
        exit(1);
    }

    // Initialiser toutes les cases à NULL
    for (int i = 0; i < TAILLE_TABLE; i++) {
        th->table[i] = NULL;
    }

    return th;
}

void construireTableHachage(TableHachage *th, Liste *Li) {
    if (Li->tete == NULL) {
        printf("Liste vide\n");
        return;
    }

    //on vide la table existante
    for (int i = 0; i < TAILLE_TABLE; i++) {
        Produit* courant = th->table[i];
        while (courant != NULL) {
            Produit* temp = courant;
            courant = courant->suivant;
            free(temp);
        }
        th->table[i] = NULL;
    }

    //on parcoule la liste de produits
    //et au fur et a mesure on remplir la table de hachage
    //avec les donne stocke dans le noeud courant
    //qu on a chaque fois il s incremente et devient courant -> suivant
    Produit* courant = Li->tete;

    while (courant != NULL) {
        //on Calcule l'index ou on va stoker le produit
        //dans la table de hachage
        //a l aide de la fonction hachage()
        int index = hachage(courant->id);

        // Créer une copie qui est independante du produit
        Produit* copie = (Produit*)malloc(sizeof(Produit));
        if (copie == NULL) {
            printf("Erreur d'allocation de memoire\n");
            return;
        }
        //dans cett copie
        //on va stoker les informations
        //qui ont etait stoker dans le courant
        copie->id = courant->id;
        strcpy(copie->nom, courant->nom);
        copie->prix = courant->prix;
        copie->quantite = courant->quantite;

        // Insérer en tête
        // les collision sont gere automatiqument
        //d abord
        //le suivant de la copie va pointe sur l ancien premier element du table [index]
        copie->suivant = th->table[index];
        //et puis on mis a jour la tete pour qu elle pointe sur le nouveau premier element
        th->table[index] = copie;
        //table[index] -> [copie] -> NULL


        //et puis le courant devient le courant suivant
        courant = courant->suivant;

        // on repete cette boucle jusqu a parcourir toute la lise des produit
        //c a dire jusqu a courant == null
    }
}

Liste *creerListe () {
    //allouer la memeoire
    Liste *Li = (Liste*)malloc(sizeof(Liste));

    if(Li == NULL) {
        printf("erreur \n");
        exit(1);
    }
    //on initialise tete et queue a null
    Li->tete = NULL;
    Li->queue = NULL;
    Li->taille = 0;
    return Li;
}

Produit *creeProduit () {
    Produit *Prd;
    //allouer la memeoire
    Prd = (Produit*)malloc(sizeof(Produit));
    if(Prd == NULL) {
        printf("erreur");
        exit(1);
    }
    //initaliser le suiavnt du produit a null
    Prd->suivant = NULL;
    return Prd;
}

Produit *EntrerDonneeProduit () {
    //on cree un produit Prd
    Produit *Prd = creeProduit();
    //on demande a l utilsateur les information du prodyuit
    printf("entrer un identifiant: ");
    scanf("%d", &Prd->id);
    printf("entrer le nom : ");
    scanf("%s", Prd->nom);
    printf("entrer le prix : ");
    scanf("%f", &Prd->prix);
    printf("entrer la quantite : ");
    scanf("%d", &Prd->quantite);
    //c est donner saisie seront stoker dans Prd de type produit
    //et enfin on retourne Prd
    return Prd;
}

void AjouterProduitFin (Liste *Li, TableHachage *th) {
    //on cree Prd de type produit
    //ou on va stoker l information du produi saiso
    Produit *Prd = EntrerDonneeProduit ();
    //si l utilisateur entre une id negatif
    if(Prd->id <= 0) {
        printf("id invalide\n");
        return;
    }
    Produit *existe = Li->tete;
    int ext = 0 ;
    while (existe != NULL) {
        if (existe->id == Prd->id || strcmp(existe->nom, Prd->nom) == 0 ) {
            ext = 1; // ID trouvé
        }
        existe = existe->suivant;
    }
    //si ex = 1 (id trouve)
    if(ext == 1) {
        printf("produit existe deja\n");
        free(Prd);
        return;
    }

    // prix negatif
    if (Prd->prix <= 0) {
        printf("Prix invalide\n");
        free(Prd);
        return;
    }

    //prix negatif
    if (Prd->quantite < 0) {
        printf("Quantite invalide\n");
        free(Prd);
        return;
    }


    //si la liset est vide
    if(Li->tete == NULL) {
        //la tete pointera sur le nouveau element
        Li->tete = Prd;
        //la queue pointera sur le nouveau element
        Li->queue = Prd;
    } else {//sinon
        //l ancienne queue sont suivant pointera sur le nouveau dernier elment(Prd)
        Li->queue->suivant = Prd;
        //la queue pointera sur Prd qui est le nouveau dernier element
        Li->queue = Prd;
    }
    //on incremente
    Li->taille ++;

    printf("le Produit ajoute a la liste avec succes \n");

    // Reconstruire la table de hachage
    construireTableHachage(th, Li);

}

void lireproduitFichier (Liste *Li, TableHachage *th) {
    //on charge les donne depuis un fichier texte 
    FILE *TP6;
    TP6 = fopen("produis.txt", "r");

    if(TP6 == NULL) {
        perror("erreur \n");
        exit(1);
    }

    //on vide d abord la liste actuelle
    Produit *courant = Li->tete;
    while(courant != NULL) {
        Produit *temp = courant;
        courant = courant->suivant;
        free(temp);
    }

    Li->tete = NULL;
    Li->queue = NULL;
    Li->taille = 0;

    //on cree une variable tmp de type produit
    //on on stokera temporairment les donne du fichier
    Produit tmp;
    //%[^|] lit tout jusqu a cracatere '|'
    while(fscanf(TP6, "%d|%[^|]|%f|%d\n", &tmp.id, tmp.nom, &tmp.prix, &tmp.quantite) == 4) {
        // on cree p de type produit
        //pour remplir a chaque fois la liste
        Produit *p = creeProduit();
        //on stoke les donne charger depuis le fichier dans p
        p->id = tmp.id;
        strcpy(p->nom, tmp.nom);
        p->prix = tmp.prix;
        p->quantite = tmp.quantite;
        p->suivant = NULL;


        //si la liste est vide
        if(Li->tete == NULL) {
            //la tete deviendra p
            Li->tete = p;
            //la queue deviendra p
            Li->queue = p;
        } else {
            //l ancienne queue sont suivant pointera sur le nouveau dernier elment(Prd)
            Li->queue->suivant = p;
            //la queue pointera sur Prd qui est le nouveau dernier element
            Li->queue = p;
        }
        Li->taille ++;
    }
    fclose(TP6);


    //on reconstriut la table de hachage
    construireTableHachage(th, Li);
}

Produit* rechercherProduitHachage(TableHachage *th, int id) {
    // Calculer l'index
    int index = hachage(id);

    //on va directement a la case
    //et on cree un courant qui va commencer au debut de la case de la table
    Produit* courant = th->table[index];

    // Parcourir liste chainé a l interieur de cette case de la table de hachage
    while (courant != NULL) {
        //si on trouve l id
        if (courant->id == id) {
            //on retourne le courant
            return courant;
        }
        //sinon
        //on passe au noeud suivant dans cette liste
        courant = courant->suivant;
    }
    return NULL;
}

Produit* rechercherProduitParnom(Liste *Li, char nom[30] ) {
    Produit* courant = Li->tete;

    // Parcourir toute la liste
    while (courant != NULL) {
        if (strcmp(courant->nom, nom) == 0) {
            return courant;  // Trouvé
        }
        courant = courant->suivant;
    }

    return NULL;
}

void EnregistrerListeProduit (Liste *Li) {
    FILE *TP6;
    TP6 = fopen("produits.txt", "w");

    if(TP6 == NULL) {
        perror("erreur \n");
        exit(1);
    }

    //le courant va commence au debut de la liste
    Produit *courant = Li->tete;
    //et a chaque fois on ecrit les donnee du courant dans le fichier
    while(courant != NULL) {
        fprintf(TP6, "%d|%s|%.2f|%d\n",courant->id, courant->nom, courant->prix, courant->quantite);
        // apres qu on a ecrit les donne de ce courant
        // on passe au noeud suiavnt
        //et on repete la meme chose
        courant = courant->suivant;
    }

    fclose(TP6);
}

void TrierListeProduitsPrix(Liste *Li) {

    if (Li->tete == NULL) {
        printf("la liste est vide");
        return;
    }

    Produit *a, *b;

    for (a = Li->tete; a != NULL; a = a->suivant) {

        for (b= a->suivant; b != NULL; b = b->suivant) {

            if (a->prix > b->prix) {

                // Stoker information de a dans un noeud temporaire
                Produit tmp = *a;
                // Copier les information de b dans a
                a->id = b->id;
                strcpy(a->nom, b->nom);
                a->prix = b->prix;
                a->quantite = b->quantite;

                //copier les information de a(ancien qui sont stocke dans tmp) dans b

                b->id = tmp.id;
                strcpy(b->nom, tmp.nom);
                b->prix = tmp.prix;
                b->quantite = tmp.quantite;
            }
        }
    }
}

void TrierListeProduitsQuantite(Liste *Li) {

    if (Li->tete == NULL) {
        printf("la liste est vide");
        return;
    }

    Produit *a, *b;

    for (a = Li->tete; a != NULL; a = a->suivant) {

        for (b= a->suivant; b != NULL; b = b->suivant) {

            if (a->quantite > b->quantite) {

                // Stoker information de a dans un noeud temporaire
                Produit tmp = *a;
                // Copier les information de b dans a
                a->id = b->id;
                strcpy(a->nom, b->nom);
                a->prix = b->prix;
                a->quantite = b->quantite;

                //copier les information de a(ancien qui sont stocke dans tmp) dans b

                b->id = tmp.id;
                strcpy(b->nom, tmp.nom);
                b->prix = tmp.prix;
                b->quantite = tmp.quantite;
            }
        }
    }
}

void TrierListeProduitsNom(Liste *Li) {

    if (Li->tete == NULL) {
        printf("la liste est vide");
        return;
    }

    Produit *a, *b;

    for (a = Li->tete; a != NULL; a = a->suivant) {

        for (b= a->suivant; b != NULL; b = b->suivant) {
            //si a->nom est apres b->nom dans l alphabet
            //et on echange leur valeur
            if (strcmp(a->nom, b->nom) > 0) {

                // Stoker information de a dans un noeud temporaire
                Produit tmp = *a;
                // Copier les information de b dans a
                a->id = b->id;
                strcpy(a->nom, b->nom);
                a->prix = b->prix;
                a->quantite = b->quantite;

                //copier les information de a(ancien qui sont stocke dans tmp) dans b

                b->id = tmp.id;
                strcpy(b->nom, tmp.nom);
                b->prix = tmp.prix;
                b->quantite = tmp.quantite;
            }
        }
    }
}

void afficherListeProduits(Liste *Li) {
    if(Li->tete == NULL) {
        printf("Liste vide\n");
        return;
    }
    //courant est au debut de la liste
    Produit *courant = Li->tete;
    //on parcour la liste
    // et a chaquue fois on affiche les donne stoke dans le courant a l utilisatiur
    printf("Liste : \n");
    while(courant != NULL) {
        printf("%d|%s|%.2f|%d\n",courant->id, courant->nom, courant->prix, courant->quantite);
        courant = courant->suivant;
    }
    printf("\n");
}

void supprimerProduit(Liste* Li, int id, TableHachage *th) {
    //liste vide
    if(Li->tete == NULL) {
        printf("Liste vide\n");
        return;
    }
    //supprimer le premier element
    if(Li->tete->id == id) {
        Produit *tmp = Li->tete;
        Li->tete = Li->tete->suivant;

        //si la liste admetter selement un element
        //il faut faire mis ajour a la queue
        if(Li->tete == NULL) {
            Li->queue = NULL;
        }

        printf("Le produit que vous avez supprime est : \n");
        printf("id: %d| %s \n", tmp->id, tmp->nom);
        free(tmp);
        Li->taille--;
        //on reconstruit le table de hachage
        construireTableHachage(th, Li);
        return;
    }

    //si le produit ne se trouve ni a la fin ni au milieu
    Produit *courant = Li->tete;
    Produit *precedent = NULL;

    while(courant !=NULL && courant->id != id) {
        precedent = courant;
        courant = courant->suivant;
    }

    if(courant == NULL) {
        printf("id: %d est introuvalble\n", id);
        return;
    }

    //on suprime l element
    //on faire precedent son suivant pointer au courant suivant
    precedent->suivant = courant->suivant;

    //si courant pointe vers la queue
    if(courant == Li->queue) {
        //il faut faire mise a jour a la queue
        Li->queue = precedent;
    }

    printf("le produit id: %d| %s est supprimé\n", courant->id, courant->nom);
    free(courant);

    Li->taille--;
    construireTableHachage(th, Li);
}

Produit* rechercherProduitParID(Liste *Li, int id) {
    Produit* courant = Li->tete;

    // Parcourir toute la liste
    while (courant != NULL) {
        if (courant->id == id) {
            return courant;  // Trouve
        }
        courant = courant->suivant;
    }

    return NULL;  // Non trouve
}

void modifierProduit(Liste *Li, TableHachage *th, int id) {
    Produit *p = rechercherProduitParID(Li, id);

    if (p == NULL) {
        printf("produit introuvable\n");
        return;
    }

    printf("\nLe Produit Actuel est \n");
    printf("ID       : %d | Nom      : %s | Prix     : %.2f | Quantité : %d \n", p->id, p->nom, p->prix, p->quantite);

    int reponse;
    printf("Modifier le nom (non = 0 | oui = 1): ");
    scanf("%d", &reponse);
    if(reponse == 1) {
        char nv[30];
        printf("nom : ");
        scanf("%s", nv);
        strcpy(p->nom, nv);

    }

    int choix;
    printf("Modifier le prix (non = 0 | oui = 1): ");
    scanf("%d", &choix);
    if(choix == 1) {
        float nouveauPrix;
        printf("nouveau prix: ");
        scanf("%f", &nouveauPrix);
        p->prix = nouveauPrix;
    }

    int ch;
    printf("Modifier la quantite (non = 0 | oui = 1): ");
    scanf("%d", &ch);
    if(ch == 1) {
        int nouvelleQte;
        printf("nouvelle quantite: ");
        scanf("%d", &nouvelleQte);
        p->quantite = nouvelleQte;
    }

    printf("Produit modifié avec succès\n");

    // Reconstruire la table de hachage
    construireTableHachage(th, Li);
}


// ==================== FONCTIONS CLIENTS ====================


ClientNode *creerClient(int id, char nom[], float total){
    ClientNode *c = (ClientNode*)malloc(sizeof(ClientNode));

    if(c == NULL){
        printf("Erreur d'allocation\n");
        exit(1);
    }

    // remplir infos client
    c->id = id; //id
    strcpy(c->nom, nom); //nom
    c->total = total; //total

    //on initialise le fils gauche et droite a zero
    c->filsg = NULL;
    c->filsd = NULL;

    return c;
}

ArbreClients *creerArbreClients(){
    ArbreClients *a = (ArbreClients*)malloc(sizeof(ArbreClients));
    if(a == NULL){
        printf("Erreur d'allocation\n");
        exit(1);
    }
    //on initialise la racine a 0
    a->racine = NULL;
    return a;
}

ClientNode* insererClient(ClientNode *r, int id, char nom[], float total){
    //verifier si l arbre est vide
    if(r == NULL){
        //si l arbre est vide
        //on cree le premier noeud
        return creerClient(id, nom, total);
    }

    // comparer les noms pour garder l'ordre alpha
    if(strcmp(nom, r->nom) < 0){
        //si le nv nom est plus petit que celui de la recine
        //on l insere a gauche
        //recursivement (la racine devien le filsg)
        r->filsg = insererClient(r->filsg, id, nom, total);
    }
    else if(strcmp(nom, r->nom) > 0){
        //si le nv nom est plus grand que celui de la recine
        //on l insere a droite
        //recursivement (la racine devien le filsd)
        r->filsd = insererClient(r->filsd, id, nom, total);
    }
    else{
        //si l utilisateur entre un client deja existant
        printf("Client existe deja.\n");
    }

    return r;
}

ClientNode* rechercherClientNom(ClientNode *r, char nom[]){
    //si l'arbre est vide (r pointe vers rien), rien à chercher
    if(r == NULL) return NULL;
    // Compare le nom recherché avec le nom du nœud actuel
    int cmp = strcmp(nom, r->nom);

    if(cmp == 0) return r;          // trouvé
    if(cmp < 0) return rechercherClientNom(r->filsg, nom);  // aller à gauche(recursif)
    return rechercherClientNom(r->filsd, nom);              // aller à droite(recursif)
}

ClientNode* rechercherClientID(ClientNode *r, int id){
    if (r == NULL)
        return NULL;

    // si l'id correspond → trouvé
    if (r->id == id)
        return r;

    // chercher dans le sous-arbre gauche
    ClientNode *g = rechercherClientID(r->filsg, id);
    if (g != NULL)
        return g;

    // sinon chercher dans le sous-arbre droit
    return rechercherClientID(r->filsd, id);
}

ClientNode* minNode(ClientNode *r){
    // Descend toujours vers le nœud le plus à gauche dans l'arbre
    //(le plus petit alphabétiquement)
    while(r->filsg != NULL){ // Tant qu'il y a un fils gauche
        //on pointe sur ce fils
        r = r->filsg;
    }
    // a la fin, r pointe vers le nœud sans fils gauche
    //c est le minimum
    return r;
}

ClientNode* supprimerClientNom(ClientNode *r, char nom[]){
    //arbre vide, rien à supprimer
    if(r == NULL) return NULL;

    // Compare le nom à supprimer avec le nœud actuel
    int cmp = strcmp(nom, r->nom);

    // Si nom plus petit
    if(cmp < 0){
        //cherche/supprime dans le sous-arbre gauche (récursif)
        r->filsg = supprimerClientNom(r->filsg, nom);
    }
    // Si nom plus grand
    else if(cmp > 0){
        //cherche/supprime dans le sous-arbre droit (récursif)
        r->filsd = supprimerClientNom(r->filsd, nom);
    }
    else{
        // cas 1: feuille
        if(r->filsg == NULL && r->filsd == NULL){
            // Cas 1 : feuille (pas d'enfants) on le libere et renvoie NULL
            free(r);
            return NULL;
        }
        // cas 2: un seul fils droit
        else if(r->filsg == NULL){
            // Cas 2: un seul fils droit on le remplace par ce fils
            //on conserve les donne du fils gauche
            ClientNode *tmp = r->filsd;
            //on libere
            free(r);
            return tmp;
        }
        // cas 3 : un seul fils gauche
        else if(r->filsd == NULL){
            //un seul fils gauche on le remplace par ce fils
            ClientNode *tmp = r->filsg; //on conserve les donne du fils g
            free(r);
            return tmp;
        }
        else{
            // cas 4 : deux fils, on prend (le min droit)
            ClientNode *succ = minNode(r->filsd);

            // copier infos successeur
            strcpy(r->nom, succ->nom);
            r->id = succ->id;
            r->total = succ->total;

            // supprimer successeur
            r->filsd = supprimerClientNom(r->filsd, succ->nom);
        }
    }

    return r;
}

void afficherClients(ClientNode *r){
    if(r != NULL){
        afficherClients(r->filsg);
        printf("%d | %s | %.2f DH\n", r->id, r->nom, r->total);
        afficherClients(r->filsd);
    }
}

void sauvegarderClient(ClientNode *r, FILE *f){
    if(r != NULL){
        fprintf(f, "%d %s %.2f\n", r->id, r->nom, r->total);
        sauvegarderClient(r->filsg, f);
        sauvegarderClient(r->filsd, f);
    }
}

void chargerClients(ArbreClients *a){
    FILE *f = fopen("clients.txt", "r");
    if(f == NULL) return;

    int id;
    char nom[30];
    float total;

    while(fscanf(f, "%d %s %f", &id, nom, &total) != EOF){
        a->racine = insererClient(a->racine, id, nom, total);
    }

    fclose(f);
}



// ==================== FONCTIONS FILE D'ATTENTE Client ====================



ClientFile* CreerFileClient(int id, char nom[], float total){
    //alloue la memeoire pour un nouveau noeud
    ClientFile *CF = (ClientFile*)malloc(sizeof(ClientFile));
    //on enregistre les information du client dans le noeud
    CF->id = id;
    strcpy(CF->nom, nom);
    CF->total = total;
    CF->suivant = NULL;
    return CF;
}

File* creerFile(){
    File *f = (File*)malloc(sizeof(File));
    //on initialise la tete et la queue de la file a null
    f->tete = NULL;
    f->queue = NULL;
    f->nef = 0;
    return f;
}

void enfilerClient(File *f, int id, char nom[], float total){
    //on cree un nouveau noeud client
    ClientFile *m = CreerFileClient(id, nom, total);
    //si la liste est vide il devient le premier element
    if(f->tete == NULL){
        //le nv element devient la tete et la queue
        f->tete = m;
        f->queue = m;
    }
    else {
        //sinon on ajoute a la fin
        //l ancien queue pointera sur le nouveau
        f->queue->suivant = m;
        f->queue = m;
    }

    f->nef++;
}

ClientFile* defilerClient(File *f) {
    if(f == NULL || f->tete == NULL) {
        printf("File vide\n");
        return NULL;
    }
    ClientFile *ptr = f->tete;
    f->tete = f->tete->suivant;
    f->nef--;
    if(f->tete == NULL) {
        f->queue = NULL;
    }
    return ptr;
}

void afficherFile(File *f){
    ClientFile *tmp = f->tete;

    printf("\n   FILE D'ATTENTE    \n");

    if(tmp == NULL){
        printf("La file est vide\n");
        return;
    }

    while(tmp != NULL){
        printf("%d | %s | %.2f DH\n", tmp->id, tmp->nom, tmp->total);
        tmp = tmp->suivant;
    }
}



// ==================== FONCTIONS PILE TRANSACTIONS ====================

Pile* creerPile() {
    Pile *p = (Pile*)malloc(sizeof(Pile));
    if(p == NULL) {
        printf("Erreur d'allocation\n");
        exit(1);
    }
    p->tete = NULL;
    p->nef = 0;
    return p;
}

Transaction* creerTransaction(int idClient, char nomClient[], int idProduit, char nomProduit[], int quantite, float sousTotal) {
    Transaction *t = (Transaction*)malloc(sizeof(Transaction));
    if(t == NULL) {
        printf("Erreur d'allocation\n");
        exit(1);
    }

    t->idClient = idClient;
    strcpy(t->nomClient, nomClient);
    t->idProduit = idProduit;
    strcpy(t->nomProduit, nomProduit);
    t->quantite = quantite;
    t->total = sousTotal;

    // Obtenir la date actuelle
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    sprintf(t->date, "%02d/%02d/%04d %02d:%02d",
            temps->tm_mday, temps->tm_mon + 1, temps->tm_year + 1900,
            temps->tm_hour, temps->tm_min);

    t->suivant = NULL;
    return t;
}

void empiler(Pile *p, int idClient, char nomClient[], int idProduit, char nomProduit[], int quantite, float sousTotal) {
    Transaction *t = creerTransaction(idClient, nomClient, idProduit, nomProduit, quantite, sousTotal);

    // Inserer en tete de pile
    t->suivant = p->tete;
    p->tete = t;
    p->nef++;

    printf("Transaction enregistree dans l'historique\n");
}

Transaction* depiler(Pile *p) {
    if(p->tete == NULL) {
        printf("Pile vide - Aucune transaction a annuler\n");
        return NULL;
    }

    Transaction *t = p->tete;
    p->tete = p->tete->suivant;
    p->nef--;

    return t;
}

void afficherHistorique(Pile *p) {
    if(p->tete == NULL) {
        printf("\nAucune transaction dans l'historique\n");
        return;
    }

    printf("\n        HISTORIQUE DES TRANSACTIONS          \n");

    Transaction *courant = p->tete;
    while(courant != NULL) {
        char nomClientGroupe[30];
        char dateGroupe[20];
        float totalGroupe = 0.0;

        strcpy(nomClientGroupe, courant->nomClient);
        strcpy(dateGroupe, courant->date);

        printf("\n  PANIER du  %s (ID: %d)  %s ---\n", nomClientGroupe, courant->idClient, dateGroupe);
        printf("Produit\t\tQte\tPrix Unitaire\tSous-Total\n");
        printf("................................................\n");

        // Grouper tant que meme client/date
        while(courant != NULL && strcmp(courant->nomClient, nomClientGroupe) == 0 && strcmp(courant->date, dateGroupe) == 0) {

            float prixUnitaire = courant->total / courant->quantite;  // Recalcul pour affichage
            printf("%s\t\t%d\t%.2f\t\t%.2f\n", courant->nomProduit, courant->quantite, prixUnitaire, courant->total);

            totalGroupe += courant->total;

            courant = courant->suivant;
        }

        printf("...............................................\n");
        printf("\ntotal: %.2f dh \n", totalGroupe);

    }
}

void sauvegarderHistorique(Pile *p) {
    FILE *f = fopen("historique.txt", "w");
    if(f == NULL) {
        printf("Erreur ouverture fichier historique\n");
        return;
    }

    Transaction *courant = p->tete;
    while(courant != NULL) {
        fprintf(f, "%d|%s|%d|%s|%d|%.2f|%s\n",
                courant->idClient, courant->nomClient, courant->idProduit, courant->nomProduit,
                courant->quantite, courant->total, courant->date);
        courant = courant->suivant;
    }

    fclose(f);
    printf("Historique sauvegarde avec succes\n");
}

// ==================== FONCTIONS PASSAGE EN CAISSE ====================

void genererTicket(int idClient, char nomClient[], Produit panier[], int nbrProduits, float totalGeneral) {
    // Crée le nom du fichier de ticket (chemin fixe + ID client)
    char nomFichier[100];
    sprintf(nomFichier, "c:\\Users\\hp\\Documents\\ticket_%d.txt", idClient);

    FILE *f = fopen(nomFichier, "w");
    if(f == NULL) {
        printf("Erreur creation ticket\n");
        return;
    }

    // En-tete du ticket
    fprintf(f, "        MINI SUPERMARCHE\n");
    fprintf(f, "............................................\n");
    fprintf(f, "le client: %s avec id: %d\n", nomClient, idClient);

    // Obtenir la date et l afficher
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    fprintf(f, "Date: %d/%d/%d %d:%d\n", temps->tm_mday, temps->tm_mon + 1, temps->tm_year + 1900, temps->tm_hour, temps->tm_min);

    fprintf(f, "............................................\n");
    fprintf(f, "Produit\t\tQte\tPrix\tTotal\n");
    fprintf(f, ".............................................\n");

    // Liste des produits
    for(int i = 0; i < nbrProduits; i++) {
        float sousTotal = panier[i].prix * panier[i].quantite;
        fprintf(f, "%s\t\t%d\t%.2f\t%.2f\n", panier[i].nom, panier[i].quantite, panier[i].prix, sousTotal);
    }

    fprintf(f, "............................................\n");
    fprintf(f, "le total est : %.2f dh\n", totalGeneral);
    fprintf(f, "............................................\n");

    fclose(f);
    printf("Ticket sauvegarde: %s\n", nomFichier);
}

void afficherTicketEcran(int idClient, char nomClient[], Produit panier[], int nbrProduits, float totalGeneral) {
    printf("        MINI SUPERMARCHE\n");
    printf(".........................................\n");
    printf("le client: %s avec id : %d\n", nomClient, idClient);
    // Obtient et affiche la date/heure actuelle
    time_t maintenant = time(NULL);
    struct tm *temps = localtime(&maintenant);
    printf("Date: %d/%d/%d %d:%d\n", temps->tm_mday, temps->tm_mon + 1, temps->tm_year + 1900, temps->tm_hour, temps->tm_min);

    printf("............................................\n");
    printf("Produit\t\tQte\tPrix\tTotal\n");
    printf("............................................\n");
    // Parcourt et affiche chaque produit du panier
    for(int i = 0; i < nbrProduits; i++) {
        float sousTotal = panier[i].prix * panier[i].quantite;
        printf("%s\t\t%d\t%.2f\t%.2f\n", panier[i].nom, panier[i].quantite, panier[i].prix, sousTotal);
    }

    printf(".........................................\n");
    printf("le total est : %.2f dh\n", totalGeneral);
}

void servirClient(File *file, Liste *Li, TableHachage *th, ArbreClients *arbre, Pile *pile) {
    // Verifier si la file est vide
    if(file->tete == NULL) {
        printf("\nAucun client en attente\n");
        return;
    }

    // on retire le client de la file
    ClientFile *client = defilerClient(file);
    if(client == NULL) {
        //si la file est vide on sort
        return;
    }

    printf("\n      SERVIR CLIENT       \n");
    printf("le Client: %s avec id: %d\n", client->nom, client->id);

    // Initialise un panier temporaire (tableau de produits)
    Produit panier[100];
    // Compteur de produits ajoutés
    int nbrProduits = 0;
    // Total des achats
    float totalGeneral = 0.0;

    int continuer = 1;
    // Boucle pour ajouter des produits
    while(continuer) {
        int idProduit;
        //on demande a l utilisateur d entrer l id du produit
        printf("\nEntrez l id du produit (0 pourquitter ): ");
        scanf("%d", &idProduit);

        if(idProduit == 0) {
            break;
        }

        // Rechercher le produit dans la table de hachage
        //et on le stocke dans p
        Produit *p = rechercherProduitHachage(th, idProduit);

        if(p == NULL) {
            printf("Produit n est pas dans la liste des produit\n");
            continue;
        }

        //on affiche le produit si on l a trouver
        printf("Produit est  trouve: %s (prix: %.2f / qte: %d)\n", p->nom, p->prix, p->quantite);

        //on demande a l utilisateur d entrer la quantite des produits
        int qte;
        printf("entrer la quantite que vous voulez: ");
        scanf("%d", &qte);

        if(qte <= 0) {
            printf("Quantite invalide\n");
            continue;
        }

        if(qte > p->quantite) {
            printf("Stock est insuffisant , il nous reste :  %d\n", p->quantite);
            continue;
        }

        // Ajouter au panier
        //on entre les information du produit que le client veux dans le panier
        panier[nbrProduits].id = p->id;
        strcpy(panier[nbrProduits].nom, p->nom);
        panier[nbrProduits].prix = p->prix;
        panier[nbrProduits].quantite = qte;

        float sousTotal = p->prix * qte; // Calcule sous-total
        totalGeneral += sousTotal; // Ajoute au total général

        printf("Ajoute au panier pour le produit %s :  %.2f dh\n",p->nom,  sousTotal);

        // Empile la transaction dans la pile (historique)
        empiler(pile, client->id, client->nom, p->id, p->nom, qte, sousTotal);

        // Enregistrer la transaction dans la pile

        nbrProduits++; // Incrémente le compteur

        //demander a l utilisateur s il veux ajouter un autre produit

        printf("\nvouler vous acheter un autre produit (1=oui, 0=non): ");
        scanf("%d", &continuer);
    }

    if(nbrProduits == 0) {
        // Si rien acheté
        printf("\nAucun achat effectue\n");
        free(client);
        return;
    }

    // Afficher et generer le ticket
    afficherTicketEcran(client->id, client->nom, panier, nbrProduits, totalGeneral);
    genererTicket(client->id, client->nom, panier, nbrProduits, totalGeneral);

    // Mettre a jour le stock dans la liste de produit
    for(int i = 0; i < nbrProduits; i++) {
        Produit *p = rechercherProduitParID(Li, panier[i].id);
        if(p != NULL) {
            p->quantite -= panier[i].quantite;// Soustrait la quantité vendue
        }
    }

    // Reconstruire la table de hachage
    construireTableHachage(th, Li);

    // Mettre a jour le total du client dans l'arbre
    ClientNode *clientNode = rechercherClientID(arbre->racine, client->id);
    if(clientNode != NULL) {
        clientNode->total += totalGeneral;
        printf("\nTotal depense par %s: %.2f DH\n", clientNode->nom, clientNode->total);
    }

    free(client);
    printf("\nla transaction est terminee avec succes\n");
}

void annulerDerniereTransaction(Pile *pile, Liste *Li, TableHachage *th, ArbreClients *arbre) {
    // Vérifie si la pile est vide (pas de transaction à annuler)
    if(pile->tete == NULL) {
        printf("\naucune transaction a annuler\n");
        return;
    }

    //Collecter toutes les transactions du dernier panier (meme client + meme date)
    Transaction *transactionsPanier[100];  /// Tableau pour stocker des produits du panier
    int nbrTransactions = 0;// Compteur de transaction
    float totalAnnule = 0.0;// Total  annuler
    char nomClientAnnule[30];// Nom du client
    int idClientAnnule; // ID du client
    char datePanier[20];// date du paanier por grouper selon les date

    // Commence par la transaction en tête (dernière)
    Transaction *courante = pile->tete;
    strcpy(nomClientAnnule, courante->nomClient);
    idClientAnnule = courante->idClient;
    strcpy(datePanier, courante->date);

    // Popper et collecter tant que meme client/date
    while(courante != NULL && nbrTransactions < 100 && strcmp(courante->nomClient, nomClientAnnule) == 0 && strcmp(courante->date, datePanier) == 0) {

        transactionsPanier[nbrTransactions] = courante;
        totalAnnule += courante->total;

        // Depiler
        // Avance la tête
        pile->tete = courante->suivant;
        //decremente le compteur
        pile->nef--;

        nbrTransactions++;
        // Passe à la suivante
        courante = pile->tete;  // Prochaine
    }

    if(nbrTransactions == 0) {
        printf("Erreur annulation\n");
        return;
    }

    printf("\n          ANNULATION PANIER COMPLET       \n");
    printf("le client %s avec l id: %d\n", nomClientAnnule, idClientAnnule);
    printf("Panier annule: %d produits, total: %.2f dh\n", nbrTransactions, totalAnnule);

    // Lister les produits annules
    printf("Produits annule :\n");
    for(int i = 0; i < nbrTransactions; i++) {
        Transaction *t = transactionsPanier[i];
        printf("le produit %d : %s avec la Qte achete: %d avec un sous total:  %.2f DH)\n", i,  t->nomProduit, t->quantite, t->total);
    }

    // Restaurer stocks pour CHAQUE produit
    for(int i = 0; i < nbrTransactions; i++) {
        Transaction *t = transactionsPanier[i];
        Produit *p = rechercherProduitParID(Li, t->idProduit);
        if(p != NULL) {
            p->quantite += t->quantite;
            printf("le Stock de  %s devient (+%d) : %d\n", p->nom, t->quantite, p->quantite);
        } else {
            printf("Produit %d introuvable\n", t->idProduit);
        }
        free(t);  // Free chaque
    }

    construireTableHachage(th, Li);

    // Ajuster total client (total global)
    ClientNode *client = rechercherClientID(arbre->racine, idClientAnnule);
    if(client != NULL) {
        client->total -= totalAnnule;
        printf("Total de %s devient (-%.2f dh) : %.2f dh\n", client->nom, totalAnnule, client->total);
    }

    printf("\nPanier annule avec succes \n");
}

// ==================== AJOUTER PANIER MANUELLEMENT ====================

void ajouterPanierManuellement(Liste *Li, TableHachage *th, ArbreClients *arbre, Pile *pile) {
    // Demander l'ID du client
    int idClient;
    printf("\n       AJOUTER PANIER MANUELLEMENT          \n");
    printf("id du client: ");
    scanf("%d", &idClient);

    // Chercher le client
    ClientNode *client = rechercherClientID(arbre->racine, idClient);

    if(client == NULL) {
        printf("Client introuvable!\n");
        return;
    }

    printf("Client: %s\n", client->nom);

    // Creer le panier
    Produit panier[100];
    int nbrProduits = 0;
    float totalGeneral = 0.0;

    int continuer = 1;

    // Boucle d'achat
    while(continuer) {
        int idProduit;
        printf("\nid du produit (0 pour quitter): ");
        scanf("%d", &idProduit);

        if(idProduit == 0) {
            break;
        }

        // Chercher le produit
        Produit *p = rechercherProduitHachage(th, idProduit);

        if(p == NULL) {
            printf("Produit introuvable!\n");
            continue;
        }

        printf("Produit: %s avec un prix: %.2f dh et un Stock de : %d\n", p->nom, p->prix, p->quantite);

        int qte;
        printf("Quantite: ");
        scanf("%d", &qte);

        if(qte <= 0 || qte > p->quantite) {
            printf("Quantite invalide\n");
            continue;
        }

        // Ajouter au panier
        panier[nbrProduits].id = p->id;
        strcpy(panier[nbrProduits].nom, p->nom);
        panier[nbrProduits].prix = p->prix;
        panier[nbrProduits].quantite = qte;

        float sousTotal = p->prix * qte;
        totalGeneral += sousTotal;

        printf("Total: %.2f dh\n", totalGeneral);

        empiler(pile, client->id, client->nom, p->id, p->nom, qte, sousTotal);

        nbrProduits++;

        printf("Continuer? (1=oui, 0=non): ");
        scanf("%d", &continuer);
    }

    if(nbrProduits == 0) {
        printf("Aucun achat\n");
        return;
    }

    // Generer le ticket
    afficherTicketEcran(client->id, client->nom, panier, nbrProduits, totalGeneral);
    genererTicket(client->id, client->nom, panier, nbrProduits, totalGeneral);

    // Mettre a jour le stock
    for(int i = 0; i < nbrProduits; i++) {
        Produit *p = rechercherProduitParID(Li, panier[i].id);
        if(p != NULL) {
            p->quantite -= panier[i].quantite;
        }
    }

    construireTableHachage(th, Li);

    // Mettre a jour le total du client
    client->total += totalGeneral;

    printf("\nTransaction terminee!\n");
    printf("Nouveau total %s: %.2f DH\n", client->nom, client->total);
}


// ==================== CHARGER TOUTES LES DONNEES ====================

void chargerTout(Liste *Li, TableHachage *th, ArbreClients *arbre) {
    // Charger les produits
    lireproduitFichier(Li, th);

    // Charger les clients
    chargerClients(arbre);

    printf("Toutes les donnees ont ete chargees\n");
}


// ==================== MENUS ====================

void menuProduits(Liste *Li, TableHachage *th) {
    int choix;

    do {
        printf("\n----------- GESTION PRODUITS ------------\n");
        printf("1) ajouter un produits\n");
        printf("2) modifier un produit\n");
        printf("3) supprimer un produit\n");
        printf("4) rechercher un produit par ID\n");
        printf("5) rechercher un produit par nom\n");
        printf("6) trier un produit\n");
        printf("7) afficher produits\n");
        printf("8) enregistre dans le fichier\n");
        printf("9) charger depuis le fichier\n");
        printf("10) retour \n");
        printf("votre choix: ");
        scanf("%d", &choix);

        switch(choix) {
            case 1: {
                AjouterProduitFin(Li, th);
            }
            break;
            case 2: {
                int id;
                printf("id : ");
                scanf("%d", &id);
                modifierProduit(Li, th, id);
            }
            break;
            case 3: {
                int id;
                printf("id : ");
                scanf("%d", &id);
                supprimerProduit(Li, id, th);
            }
            break;
            case 4: {
                int id;
                printf("id : ");
                scanf("%d", &id);
                Produit *p = rechercherProduitHachage(th, id);
                if(p == NULL) {
                    printf("produit non trouvé\n");
                } else {
                    printf("%d|%s|%.2f|%d\n",p->id, p->nom, p->prix, p->quantite);
                }
            }
            break;
            case 5: {
                char nom [30];
                printf("nom : ");
                scanf("%s", nom);
                Produit *p = rechercherProduitParnom(Li, nom);
                if(p != NULL) {
                    printf("%d|%s|%.2f|%d\n",p->id, p->nom, p->prix, p->quantite);
                } else {
                    printf("le produit %s est non trouvé\n", nom);
                }
            }
            break;
            case 6: {
                int tr;
                printf("trier (1 = nom | 2 = prix | 3 = quantite) : ");
                scanf("%d", &tr);

                if(tr == 1) {
                    TrierListeProduitsNom(Li);
                }else if(tr == 2) {
                    TrierListeProduitsPrix(Li);
                }else if(tr == 3) {
                    TrierListeProduitsQuantite(Li);
                }else {
                    printf("erreur\n");
                }
            }
            break;
            case 7: afficherListeProduits(Li);
            break;
            case 8: EnregistrerListeProduit (Li);
            break;
            case 9: lireproduitFichier (Li, th);
            break;
            case 10: printf("\n");
            break;
            default : printf("erreur\n");
        }
    } while (choix != 10);
}

void menuClients(ArbreClients *a, File *file) {
    int choix, id;
    char nom[30];
    float total;
    ClientNode *res;

    do{
        printf("\n------ MENU CLIENT ------\n");
        printf("1 : Ajouter client\n");
        printf("2 : Rechercher client (par nom)\n");
        printf("3 : Supprimer client (par nom)\n");
        printf("4 : Afficher clients tries\n");
        printf("5 : Sauvegarder clients\n");
        printf("6 : Rechercher client (par ID)\n");
        printf("7 : charger \n");
        printf("8 : Mettre en file d'attente\n");
        printf("9 : Afficher file\n");
        printf("0 : Retour\n");
        printf("Choix : ");
        scanf("%d",&choix);

        while(choix<0 || choix>9){
            printf("Votre choix doit etre entre 0 et 9 : ");
            scanf("%d" , &choix);
        }

        switch(choix){

        case 1:
            printf("ID : ");
            scanf("%d",&id);

            if(rechercherClientID(a->racine, id) != NULL){
                printf("ID existe deja. Ajout impossible.\n");
                break;
            }else if(id <= 0) {
                printf("invalide\n");
            }

            printf("Nom : ");
            scanf("%s", nom);



            total = 0;

            a->racine = insererClient(a->racine, id, nom, total);
            printf("Client ajoute.\n");
            break;

        case 2:
            printf("Nom recherche : ");
            scanf("%s", nom);

            res = rechercherClientNom(a->racine, nom);

            if(res == NULL)
                printf("Client introuvable.\n");
            else
                printf("%d | %s | %.2f DH\n", res->id, res->nom, res->total);
            break;

        case 3:
            printf("Nom a supprimer : ");
            scanf("%s", nom);

            a->racine = supprimerClientNom(a->racine, nom);
            printf("Client supprime.\n");
            break;

        case 4:
            printf("\n--- Liste triee (par nom) : ---\n");
            afficherClients(a->racine);
            break;

        case 5:{
            FILE *f = fopen("clients.txt", "w");
            sauvegarderClient(a->racine, f);
            fclose(f);
            printf("Sauvegarde terminee.\n");
            break;
        }
        case 6: {
            printf("ID recherche : ");
            scanf("%d", &id);
            res = rechercherClientID(a->racine, id);
            if(res == NULL) {
                printf("Client introuvable.\n");
            } else {
                printf("%d | %s | %.2f DH\n", res->id, res->nom, res->total);
            }
            break;
        }
        case 7: {
            chargerClients(a);
            printf("Chargement termine.\n");
        }
        break;
        case 8: {
            printf("ID du client à mettre en file : ");
            scanf("%d", &id);
            res = rechercherClientID(a->racine, id);
            if(res == NULL){
                printf("Client introuvable.\n");
            } else {
                enfilerClient(file, res->id, res->nom, res->total);
                printf("Client mis en file d'attente.\n");
            }
        }
        break;
        case 9: {
            afficherFile(file);
        }
        break;
    }
    }while(choix != 0);

}

void menuPassageEnCaisse(File *file, Liste *Li, TableHachage *th, ArbreClients *arbre, Pile *pile) {

    int choix;

    do {
        printf("\n===== PASSAGE EN CAISSE =====\n");
        printf("1) Servir le prochain client\n");
        printf("2) Ajouter panier manuellement\n");
        printf("3) Afficher file d'attente\n");
        printf("4) Afficher historique\n");
        printf("5) Annuler derniere transaction\n");
        printf("6) Sauvegarder historique\n");
        printf("0) Retour\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch(choix) {
            case 1:
                servirClient(file, Li, th, arbre, pile);
                break;
            case 2:
                ajouterPanierManuellement(Li, th, arbre, pile);
            break;
            case 3:
                afficherFile(file);
                break;

            case 4:
                afficherHistorique(pile);
                break;

            case 5:
                annulerDerniereTransaction(pile, Li, th, arbre);
                break;

            case 6:
                sauvegarderHistorique(pile);
                break;

            case 0:
                printf("Retour au menu principal\n");
                break;

            default:
                printf("Choix invalide\n");
        }

    } while(choix != 0);


}


// ==================== PROGRAMME PRINCIPAL ====================

int main() {
    // Initialisation des structures
    Liste *listeProduits = creerListe();
    TableHachage *tableHachage = creerTableHachage();
    ArbreClients *arbreClients = creerArbreClients();
    File *fileAttente = creerFile();
    Pile *pileHistorique = creerPile();

    int choix;

    printf("  GESTION MINI SUPERMARCHE\n");

    do {
        printf("\n        MENU PRINCIPAL\n");
        printf("1) Gestion produit\n");
        printf("2) Gestion client\n");
        printf("3) Passage en caisse\n");
        printf("4) charger tout\n");
        printf("5) Quitter\n");
        printf("Votre choix: ");
        scanf("%d", &choix);

        switch(choix) {
            case 1:
                menuProduits(listeProduits, tableHachage);
                break;
            case 2:
                menuClients(arbreClients, fileAttente);
            break;
            case 3:
                menuPassageEnCaisse(fileAttente, listeProduits, tableHachage, arbreClients, pileHistorique);
            break;
            case 4:
                chargerTout(listeProduits, tableHachage, arbreClients);
            break;
            case 5:
                printf("\n");
                printf("au revoir \n");
                break;
            default:
                printf("Choix invalide\n");
        }
    } while(choix != 5);
    system("pause");
    return 0;
}
