
/* 
 * Copyright (C) 2000  \RisK\
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /* language stuff */
#define FAILED_TIMEOUT		get_language(0xa390)
#define FAILED_SERVER		get_language(0xa391)
#define FAILED_MAXLEV		get_language(0xa392)
#define FAILED_MAXSER		get_language(0xa393)
#define FAILED_REMOTE		get_language(0xa394)

 /* more language stuff */
#define MISC_SKIP_CACHED	get_language(0xa3a0)
#define MISC_CAN_NOT		get_language(0xa3a1)
#define MISC_TRY_CACHED		get_language(0xa3a2)
#define MISC_FLOOD_THR		get_language(0xa3a3)
#define MISC_REMEBER		get_language(0xa3a4)
#define MISC_FORWARD_IGN	get_language(0xa3a5)

 /* flags used with stat */
#define QUARYIP       0x00000001 /*  query in process */
#define TIMEOUT       0x00000002 /*  timeout */
#define TCACHED       0x00000004 /*  tree cached */
#define FORWARD       0x00000008 /*  forwarding links request */

 /* flags used with options */
#define SHOW_D        0x00000001 /*  show server descriptions */
#define SHOW_H        0x00000002 /*  show hups names */
#define DO_EXT        0x00000004 /*  extract list */
#define UCACHE        0x00000008 /*  use cached tree */

#define MAXIDX 10 /* max idx to be stacked */
#define MAXSER 100 /* max servers */

#define BUFSIZE 256

#define MAXLEV 20
#define HUBP 50  /*  hup name pos from left */

typedef struct sLink sLink;
struct sLink {
	char *from;
	char *to;
	int used;
};

struct sLink slink[MAXSER];

static char *hups[MAXSER]; /* hups names */
static char *desc[MAXSER]; /* descriptions of servers */

static char *treeout[MAXSER+1];

static int order[MAXSER]; /*  order of descriptions/hup names */
static int stat=0; /*  status flags */
static int tlen; /*  treeout length */
static int serc; /*  number of servers */
static int idxc; /*  number of users waiting for graph */
static int idxl[MAXIDX]; /*  list of idx to get graph */
static int idxo[MAXIDX]; /*  graph options for each idx */


static int timeout = 60;
static int counter;
static char flags[12] = "";
static int max_thr = 2;
static int max_time = 90;
static int incr_thr;
static int thr;

static void irctree_checks();
static void display_tree(int idx, int option);
static void t_printf(char *text,...);
static int draw_tree();

static int raw_365(char *from, char *msg);

static int raw_402(char *from, char *msg);
static void lrange(char *destination, char *source, int p1, int size);
static void lindex(char *destination, char *source, int p1, int size);
static int raw_364(char *from, char *msg);

static int cmd_irctree(struct userrec *u, int idx, char *args);
