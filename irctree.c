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

#define MAKING_IRCTREE
#define MODULE_NAME "irctree"
#include "../module.h"
#include "../server.mod/server.h"
#include "irctree.h"
#undef global

static Function * global = NULL, *server_funcs = NULL;

static void irctree_checks()
{
	int i;
	
	Context;
	if (stat&QUARYIP)
	{
		counter++;
		if (counter>timeout)
		{
			for (i = 0; i < idxc; i++)
			{
				if (server_online) dprintf(idxl[i], FAILED_TIMEOUT);
				else dprintf(idxl[i], FAILED_SERVER);
			}
			serc=0;
			stat&=~QUARYIP;
			stat&=~FORWARD;
		}
	}
	if (thr<max_thr && max_thr)
	{
		incr_thr++;
		if (incr_thr>(max_time/max_thr))
		{
			thr++;
			incr_thr=0;
		}
	}
}

static void display_tree(int idx, int options)
{
	int recn;
	
	Context;
	for (recn = 0; recn < tlen; recn++)
	{
		int i;
		char tmp[HUBP], nwrk[BUFSIZE];
		if (recn == tlen-1)
		{
			dprintf(idx, "%s\n", treeout[recn]);
			break;
		}
		if (options&SHOW_H && hups[order[recn]])
		{
			int s;
			s = HUBP-(strlen(treeout[recn]));
			for (i=0; i < s; i++) tmp[i] = ' ';
			tmp[i] = '\0';
			if (!i) strcpy(tmp, "  ");
			dprintf(idx, "%s%s%s\n", treeout[recn], tmp, hups[order[recn]]);
		}
		else dprintf(idx, "%s\n", treeout[recn]);
		if ((options&DO_EXT) || (options&SHOW_D)) {
			int l, c;
			char work[BUFSIZE];
			strcpy(work, treeout[recn]);
			if (work[0] == ' ')
			{
				for (i=0;;i++)
				{
					if (work[i] == '`') {
						i -= 4;
						break;	
					}
					if (work[i] == '-') {
						i = 0;
						break;	
					}
				}
				c = i+5;
			}
			else c = 4;
			strcpy(work, treeout[recn+1]);
			if (recn+1 != tlen-1)
			{
				for (i=0;; i++)
				{
					if (work[i] == '-') break;
					if (work[i] == '`') {
						nwrk[i] = '|';	
					} else nwrk[i] = work[i];
				}
				nwrk[i] = '\0';
			}
			else nwrk[0] = '\0';
			l=c-strlen(nwrk);
			for (i=0; i < l; i++) tmp[i] = ' ';
			tmp[i] = '\0';
		}
		if (options&SHOW_D) dprintf(idx, "%s%s%s\n", nwrk, tmp, desc[order[recn]]);
		if (options&DO_EXT) dprintf(idx, "%s \n", nwrk);
	}
}

static void t_printf(char *text,...)
{
	char buf[BUFSIZE];
	va_list argp;
	
	Context;
	va_start(argp,text);
	vsprintf(buf,text,argp);
	treeout[tlen]=nmalloc(strlen(buf)+1);
	strcpy(treeout[tlen],buf);
	tlen++;
}

static int draw_tree()
{
	int i, more = 1, hops = 0, lev=0, coun=0, mark[MAXLEV];
	char sl[MAXLEV][BUFSIZE];
	
	Context;
	for (i = 0; i < MAXSER+1; i++) if (treeout[i])
	{
		nfree(treeout[i]);
		treeout[i]=NULL;
	}
	tlen=0;
	while(more)
	{
		int ok=0;
		if (lev == MAXLEV) return 0;
		if (lev == 0)
		{
			strcpy(sl[0], slink[0].from);
			t_printf(slink[0].from);
			slink[0].used=1;
			lev++;
			coun++;
		}
		for (i = 0; i < serc; i++) if (!strcmp(sl[lev-1], slink[i].to) && !slink[i].used)
		{
			int e, imark = 0;
			char work[BUFSIZE], pre[3];
			strcpy(sl[lev], slink[i].from);
			ok=0;
			for (e = 0; e < serc; e++) if (!strcmp(sl[lev-1], slink[e].to) && e!=i && !slink[e].used) {
				strcpy(pre,"|-");
				mark[lev] = 1;
				ok=1;
			}
			if (!ok)
			{
				strcpy(pre,"`-");
				mark[lev] = 0;
			}
			work[0]='\0';
			for (e = 1; e < lev; e++)
			{
				if (mark[e])
				{
					strcpy(work + imark, "|   ");
				}
				else
				{
					strcpy(work + imark, "    ");
				}
				imark += 4;
			}
			hops += lev;
			order[coun] = i;
			t_printf("  %s%s%s", work, pre, slink[i].from);
			slink[i].used=1;
			ok=1;
			coun++;
			lev++;
			break;
		}
		if (!ok) {
			lev--;
			if (lev==0) more=0;
		}
	}
	t_printf("Average hops: %3.1f, total servers: %d", ((float) hops) / ((float) serc - 1), serc);
	return 1;
}

static int raw_365(char *from, char *msg)
{	
	int i;
	
	Context;
	if(!(stat&QUARYIP)) return 0;
	if (!draw_tree()) {
		for (i = 0; i < idxc; i++) dprintf(idxl[i], FAILED_MAXLEV);
		stat=0;
		return 0;
	}
	else stat=TCACHED;
	for (i = 0; i < MAXSER; i++)
	{
		if (slink[i].from)
		{
			nfree(slink[i].from);
			slink[i].from=NULL;
		}
		if (slink[i].to)
		{
			nfree(slink[i].to);
			slink[i].to=NULL;
		}
	}
	for (i = 0; i < idxc; i++) display_tree(idxl[i], idxo[i]);
	return 0;
}

static int raw_402(char *from, char *msg)
{
	int i;
	
	Context;
	if(!(stat&QUARYIP) && !(FORWARD)) return 0;
	for (i = 0; i < idxc; i++) dprintf(idxl[i], FAILED_REMOTE);
	stat&=~QUARYIP;
	stat&=~FORWARD;
	return 0;
}

static void lrange(char *destination, char *source, int p1, int size)
{
	char *dst = destination, *src = source;
	int a1=0, a2=0, a3=0;
	while (a1<strlen(source) && a3 != size)
	{
		if (*(src+a1)==' ') a2++;
		if (a2==p1 || a2>p1)
		{
			*dst++ = *(src+a1);
			a3++;
		}
		a1++;
	}
	*dst = '\0';
}

static void lindex(char *destination, char *source, int p1, int size)
{
	char *dst = destination, *src = source;
	int p2=(p1+1), a1=0, a2=0, a3=0;
	Context;
	while (a1<strlen(source) && a3 != size)
	{
		if (*(src+a1)==' ')
		{
			a2++;
			if (a2==p2) break;
		} else if (a2==p1)
		{
			*dst++ = *(src+a1);
			a3++;
		}
		a1++;
	}
	*dst = '\0';
}

static int raw_364(char *from, char *msg)
{
	int i;
	char buf[BUFSIZE];
	
	Context;
	if(!(stat&QUARYIP)) return 0;
	
	if (serc == MAXSER) {
		for (i = 0; i < idxc; i++) dprintf(idxl[i], FAILED_MAXSER);
		stat=0;
		return 0;
	}
	if (!serc) for (i = 0; i < MAXSER; i++)
	{
		if (hups[i])
		{
			nfree(hups[i]);
			hups[i]=NULL;
		}
		if (desc[i])
		{
			nfree(desc[i]);
			hups[i]=NULL;
		}
		if (slink[i].from)
		{
			nfree(slink[i].from);
			slink[i].from=NULL;
		}
		if (slink[i].to)
		{
			nfree(slink[i].to);
			slink[i].to=NULL;
		}
		slink[i].used=0;
	}
	
	stat&=~FORWARD;
	
	lindex(buf, msg, 1, sizeof(buf));
	Context;
	slink[serc].from=nmalloc(strlen(buf)+1);
	strcpy(slink[serc].from,buf);
	lindex(buf, msg, 2, sizeof(buf));
	Context;
	slink[serc].to=nmalloc(strlen(buf)+1);
	strcpy(slink[serc].to,buf);
	lindex(buf, msg, 4, sizeof(buf));
	Context;
	if (buf[0] == '[') {
		i=1;
		while(buf[i])
		{
			buf[i-1] = buf[i];
			i++;
		}
		buf[i-2]= '\0';
		Context;
		hups[serc]=nmalloc(strlen(buf)+8);
		Context;
		snprintf(hups[serc], strlen(buf)+8 ,"(hup: %s)", buf);
		Context;
		lrange(buf, msg, 5, sizeof(buf));
	}
	else lrange(buf, msg, 4, sizeof(buf));
	Context;
	desc[serc]=nmalloc(strlen(buf)+1);
	strcpy(desc[serc],buf);
	Context;
	serc++;
	return 0;
}

static int cmd_irctree(struct userrec *u, int idx, char *args)
{
	int i, options=0, c=0, d=0, use_rs=0, roption=0;
	char used_args[40], remote_sv[32];
	
	Context;
	if (flags[0])
	{
		int ok=0, f;
		struct flag_record plus, minus, user;
		user.match = FR_GLOBAL;
		get_user_flagrec(u, &user, NULL);
		plus.match = user.match;
		break_down_flags(flags, &plus, &minus);
		f = (minus.global || minus.udef_global);
		if (flagrec_eq(&plus, &user))
		{
			Context;
			if (!f) ok = 1;
			else
			{
				minus.match = plus.match ^ (FR_AND | FR_OR);
				if (!flagrec_eq(&minus, &user)) ok = 1;
			}
		}
		if (!ok)
		{
			dprintf(idx, MISC_NOSUCHCMD);
			return 0;
		}
	}
	if (args[0]) {
		used_args[c++] = '-';
		for (i = 0; i < strlen(args); i++) {
			if (args[i] == ' ')
			{
				roption=0;
				if (use_rs==1) use_rs=2;
				else if (use_rs==3)
				{
					use_rs=4;
					remote_sv[d] = '\0';
				}
			}
			else if (args[i] == '-')
			{
				roption=1;
			}
			else if (args[i] != ' ' && (use_rs==2 || use_rs==3))
			{
				use_rs=3;
				if (d>30)
				{
					use_rs=4;
					remote_sv[d] = '\0';
				}
				else remote_sv[d++] = args[i];
			}
			else if (args[i] == 'e' && !(options&DO_EXT) && roption)
			{
				options|=DO_EXT;
				used_args[c++] = 'e';
			}
			else if (args[i] == 'd' && !(options&SHOW_D) && roption)
			{
				options|=SHOW_D;
				used_args[c++] = 'd';
			}
			else if (args[i] == 'h' && !(options&SHOW_H) && roption)
			{
				options|=SHOW_H;
				used_args[c++] = 'h';
			}
			else if (args[i] == 'c' && !(options&UCACHE) && roption)
			{
				options|=UCACHE;
				used_args[c++] = 'c';
			}
			else if (args[i] == 'f' && !use_rs && roption) use_rs=1;
			else if (args[i] != ' ')
			{
				dprintf(idx, "%s: irctree [-cdeh] [-f <remote server>]\n", MISC_USAGE);
				return 0;
			}
		}
		if (use_rs==3)
		{
			remote_sv[d] = '\0';
			use_rs=4;
		}
		if (use_rs==4 && !(options&UCACHE))
		{
			used_args[c++] = 'f';
			used_args[c++] = ' ';
			for (i = 0; i < d; i++) used_args[c++]=remote_sv[i];
		}
		if (!options && !use_rs) used_args[0] = '\0';
		else used_args[c] = '\0';
	}
	else used_args[0] = '\0';
	if (options&UCACHE)
	{
		if (!(stat&TCACHED))
		{
			dprintf(idx, MISC_SKIP_CACHED);
			return 0;
		}
		if (use_rs==4) dprintf(idx, MISC_CAN_NOT);
		putlog(LOG_CMDS, "*", "#%s# irctree %s", dcc[idx].nick, used_args);
		display_tree(idx, options);
		return 0;
	}
	if (!server_online) {
		dprintf(idx, MISC_TRY_CACHED);
		for (i = 0; i < idxc; i++) {
			dprintf(idxl[i], FAILED_SERVER);
		}
		stat&=~QUARYIP;
		serc=0;
		return 0;
	}
	if (!thr && max_thr)
	{
		dprintf(idx, MISC_FLOOD_THR);
		return 0;
	}
	if (stat&QUARYIP) {
		for (i = 0; i < idxc; i++) {
			if (idxl[i] == idx) {
				dprintf(idx, MISC_REMEBER);
				return 0;
			}
		}
		if (use_rs==4) dprintf(idx, MISC_FORWARD_IGN);
		idxl[idxc]=idx;
		idxo[idxc]=options;
		idxc++;
	} else {
		if (use_rs==4)
		{
			stat|=FORWARD;
			dprintf(DP_MODE, "LINKS %s *\n", remote_sv);
		}
		else
		{
			dprintf(DP_MODE, "LINKS\n");
		}
		serc=0;
		counter=0;
		thr--;
		idxl[0]=idx;
		idxo[0]=options;
		idxc++;
		stat|=QUARYIP;
		stat&=~TIMEOUT;
	}
	putlog(LOG_CMDS, "*", "#%s# irctree %s", dcc[idx].nick, used_args);
	return 0;
}

static int irctree_expmem()
{
	int i, size = 0;

	Context;
	for (i = 0; i < MAXSER; i++)
	{
		if (hups[i]) size += strlen(hups[i])+1;
		if (desc[i]) size += strlen(desc[i])+1;
	}
	for (i = 0; i < MAXSER+1; i++) if (treeout[i]) size += strlen(treeout[i])+1;
	return size;
}

static void irctree_report(int idx, int details)
{
	Context;
	if (details) dprintf(idx, "    Using %d bytes of memory\n", irctree_expmem());
}

static cmd_t C_raw[] =
{
	{"364",		"",	(IntFunc) raw_364,	NULL},
	{"365",		"",	(IntFunc) raw_365,	NULL},
	{"402",		"",	(IntFunc) raw_402,	NULL},
	{NULL,		NULL,	NULL,			NULL}
};

static cmd_t C_dcc[] =
{
	{"irctree",	"",	cmd_irctree,		NULL},
	{NULL,		NULL,	NULL,			NULL}
};

static tcl_ints irctree_tcl_ints[] =
{
	{"irctree-timeout",	&timeout, 0},
	{0, 0, 0}
};

static tcl_strings irctree_tcl_strings[] =
{
	{"irctree-flags", flags, 12, 0},
	{0, 0, 0, 0}
};

static tcl_coups irctree_tcl_coups[] =
{
	{"irctree-max", &max_thr, &max_time},
	{0, 0, 0},
};

static char *irctree_close();

static char *irctree_close()
{
	Context;
	rem_builtins(H_dcc, C_dcc);
	rem_builtins(H_raw, C_raw);
	rem_tcl_ints(irctree_tcl_ints);
	Context;
	rem_tcl_strings(irctree_tcl_strings);
	rem_tcl_coups(irctree_tcl_coups);
	del_hook(HOOK_SECONDLY, (Function) irctree_checks);
	Context;
	rem_help_reference("irctree.help");
	del_lang_section("irctree");
	module_undepend(MODULE_NAME);
	return NULL;
}

char *irctree_start(Function * global_funcs);

static Function irctree_table[] =
{
	(Function) irctree_start,
	(Function) irctree_close,
	(Function) irctree_expmem,
	(Function) irctree_report,
};

char *irctree_start(Function * global_funcs)
{
	global = global_funcs;
	Context;
	module_register(MODULE_NAME, irctree_table, 1, 6);
	if (!(server_funcs = module_depend(MODULE_NAME, "server", 1, 4)))
		return "You need the server module to use the irctree module.";
  if (!module_depend(MODULE_NAME, "eggdrop", 108, 4)) {
        module_undepend(MODULE_NAME);
        return "This module requires Eggdrop 1.8.4 or later.";
  }
	Context;
	add_builtins(H_dcc, C_dcc);
	add_builtins(H_raw, C_raw);
	Context;
	add_tcl_ints(irctree_tcl_ints);
	add_tcl_strings(irctree_tcl_strings);
	add_tcl_coups(irctree_tcl_coups);
	thr=max_thr;
	add_hook(HOOK_SECONDLY, (Function) irctree_checks);
	Context;
	add_help_reference("irctree.help");
	add_lang_section("irctree");
	return NULL;
}
