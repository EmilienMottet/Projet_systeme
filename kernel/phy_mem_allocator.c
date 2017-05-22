#include "phy_mem_allocator.h"
#include "process.h"

static LIST_HEAD(pages_vides);
static LIST_HEAD(pages_allouees);

void create_list_pages_vides() {

  for (int i = 0; i < 1000; i++) {
    page_vide* page = malloc(sizeof(*page));
	page->liens.prev = 0;
	page->liens.next = 0;
	page->priorite=0;
    page->adr = memalign(K_4KiB, K_4KiB);
    assert(page->adr != NULL);
    queue_add(page,&pages_vides,page_vide,liens,priorite);
  }
}

void desalloue_pages_process(int pid) {
	// récupérer adresse
	// ajouter à la liste des pages vides
	// supprimer l'adresse
	// faire ça pour toutes les adresses correspondant au pid
	// supprimer la page allouee

	// parcours de la liste des pages_allouees, stop quand on a trouvé le processus correspondant au pid demandé
	page_allouee* page;
	queue_for_each(page, &pages_allouees, page_allouee, liens) {
		if (page->pid == pid) {
			break;
		}
	}

	if (page->pid == pid) {
		link* liste_adr = &page->adresses;
		while (!queue_empty(liste_adr))
		{
			adresse* ad = queue_out(liste_adr, adresse, liens);
			page_vide* page_v = malloc(sizeof(*page_v));
			page_v->liens.prev = 0;
			page_v->liens.next = 0;
			page_v->adr = ad->adresse_phy;
			queue_add(page_v, &pages_vides,page_vide,liens,priorite);
			free(ad); // libere element de chainage
		}
		// suppression de la page dans la liste des pages allouées
		queue_del(page, liens);
		// libere element de chainage
		free(page);
	}
}

void alloue_pages_process(int pid, int nb_pages, PERMISSION perm) {

	page_allouee* page = malloc(sizeof(*page));
	if (page == NULL) return;
	page->liens.prev = 0;
	page->liens.next = 0;
	page->perm = perm;
	page->pid = pid;
	queue_add(page, &pages_allouees, page_allouee, liens, priorite);

	link* liste_adr = &page->adresses;

	INIT_LIST_HEAD(liste_adr);

  for (int i = 0; i < nb_pages; i++) {
		// récupération de l'adresse d'une des pages vides. La page vide utilisée est supprimée de la liste des pages vides.
		page_vide* page = queue_out(&pages_vides, page_vide, liens);
		if (page == NULL) {
			// dans le cas où il n'y a plus de pages libres, on désalloue les pages qu'on vient d'utiliser
			desalloue_pages_process(pid);
			return;
		} else {
			adresse* ad = malloc(sizeof(*ad));
			ad->liens.prev = 0;
			ad->liens.next = 0;
			ad->adresse_phy = page->adr;
			ad->adresse_virt = (void*)(K_BASE_VIRT_ADR + i*K_4KiB);
			ad->perm=perm;
			// libere element de chainage libre
			free(page);
			// ajout de l'adresse dans la liste des adresses de la page allouée
			queue_add(ad,liste_adr,adresse,liens,priorite);
		}
  }
}

// Fonction qui compte le nombre de pages allouées à un processus, identifié par son pid, et qui retourne ce nombre de pages (entier).
// Si le processus n'est pas alloué, la fonction retourne -1.
int get_nb_pages(int pid) {
	page_allouee* page;
	queue_for_each(page, &pages_allouees, page_allouee, liens) {
		if (page->pid == pid) {
			break;
		}
	}

	if (page->pid == pid) {
		int nb_pages = 0;
		link* liste_adr = &page->adresses;
		adresse* adr;
		queue_for_each(adr, liste_adr, adresse, liens) {
			nb_pages++;
		}
		return nb_pages;
	} else {
		return -1;
	}
}

// fonction get_adr_phy(pid, int no_de_la_page)
void* get_adr_phy(int pid, int no_page) {
	page_allouee* page;
	queue_for_each(page, &pages_allouees, page_allouee, liens) {
		if (page->pid == pid) {
			break;
		}
	}

	if (page->pid == pid) {
		int index_page = 0;
		link* liste_adr = &page->adresses;
		adresse* adr;
		queue_for_each(adr, liste_adr, adresse, liens) {
			if (index_page == no_page) {
				break;
			}
			index_page++;
		}
		return adr->adresse_phy;
	} else {
		return NULL;
	}
}

// fonction get_adr_virt(pid, int no_de_la_page)
void* get_adr_virt(int pid, int no_page) {
	page_allouee* page;
	queue_for_each(page, &pages_allouees, page_allouee, liens) {
		if (page->pid == pid) {
			break;
		}
	}

	if (page->pid == pid) {
		int index_page = 0;
		link* liste_adr = &page->adresses;
		adresse* adr;
		queue_for_each(adr, liste_adr, adresse, liens) {
			if (index_page == no_page) {
				break;
			}
			index_page++;
		}
		return adr->adresse_virt;
	} else {
		return NULL;
	}
}


// fonction get_adr(pid, int no_de_la_page)
adresse* get_adr(int pid, int no_page)
{
	page_allouee* page;
	queue_for_each(page, &pages_allouees, page_allouee, liens) {
		if (page->pid == pid) {
			break;
		}
	}

	if (page->pid == pid) {
		int index_page = 0;
		adresse* adr;
		queue_for_each(adr, &page->adresses, adresse, liens) {
			if (index_page == no_page) {
				break;
			}
			index_page++;
		}
		return adr;
	} else {
		return NULL;
	}
}

