/* CVS: Last edit by $Author: rr9 $ on $Date: 2000/09/16 17:41:06 $ */
/* File: wizard2.c */

/* Purpose: Wizard commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(void)
{
	int min_value, max_value, i, j, percent;

	min_value = (PY_MAX_LEVEL * 3 * (p_ptr->hitdie - 1)) / 8;
	min_value += PY_MAX_LEVEL;

	max_value = (PY_MAX_LEVEL * 5 * (p_ptr->hitdie - 1)) / 8;
	max_value += PY_MAX_LEVEL;

	player_hp[0] = p_ptr->hitdie;

	/* Rerate */
	while (1)
	{
		/* Collect values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			/* Add in racial hit dice */
			j = randint(rp_ptr->r_mhp);
			player_hp[i] = player_hp[i - 1] + j;

			/* If class hit dice is non zero - add it on */
			if (cp_ptr->c_mhp)
			{
				player_hp[i] += randint(cp_ptr->c_mhp);
			}
		}

		/* Legal values */
		if ((player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}

	percent = (int)(((long)player_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * p_ptr->hitdie +
		((PY_MAX_LEVEL - 1) * (p_ptr->hitdie + 1))));


	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	msg_format("Current Life Rating is %d/100.", percent);
}


#ifdef ALLOW_WIZARD

/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_create_named_art(int a_idx)
{
	/* Create the artifact */
	create_named_art(a_idx, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(void)
{
	/* Oops */
	msg_print("Oops.");
	(void)probing();
}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
static void do_cmd_summon_horde(void)
{
	int wy = py, wx = px;
	int attempts = 1000;
	cave_type *c_ptr;


	while (--attempts)
	{
		scatter(&wy, &wx, py, px, 3, 0);

		/* paranoia */
		if (!in_bounds2(wy, wx)) continue;

		c_ptr = area(wy, wx);
		if (cave_naked_grid(c_ptr)) break;
	}

	(void)alloc_horde(wy, wx);
}

#endif /* MONSTER_HORDES */


/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}


/*
 * Output a rarity graph for a type of object.
 */
static void prt_alloc(byte tval, byte sval, int row, int col)
{
	int i, j;
	int lev;
	int home = 0;
	u32b maxd = 1, maxr = 1, maxt = 1;
	u32b rarity[MAX_DEPTH];
	u32b total[MAX_DEPTH];
	u32b display[20];
	byte c = TERM_WHITE;
	cptr r = "+--common--+";
	object_kind *k_ptr;


	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Wipe the tables */
	(void)C_WIPE(rarity, MAX_DEPTH, u32b);
	(void)C_WIPE(total, MAX_DEPTH, u32b);
	(void)C_WIPE(display, 20, u32b);

	/* Scan all entries */
	for (i = 0; i < MAX_DEPTH; i++)
	{
		/* Base level */
		lev = ((i * (GREAT_OBJ - 1)) + (1 + i * 5433L / 1000)) / GREAT_OBJ;

		for (j = 0; j < alloc_kind_size; j++)
		{
			/* Objects are sorted by depth */
			if (table[j].level > lev) break;

			/* Acquire this kind */
			k_ptr = &k_info[table[j].index];

			/* Accumulate probabilities */
			total[i] += table[j].prob1;

			/* Accumulate probabilities */
			if ((k_ptr->tval == tval) && (k_ptr->sval == sval))
			{
				home = k_ptr->level;
				rarity[i] += table[j].prob1;
			}
		}
	}

	/* Find maxima */
	for (i = 0; i < MAX_DEPTH; i++)
	{
		if (rarity[i] > maxr) maxr = rarity[i];
		if (total[i] > maxt) maxt = total[i];
	}

	/* Simulate a log graph */
	if (maxt / maxr > 32)
	{
		c = TERM_L_WHITE;
		r = "+-uncommon-+";
	}
	if (maxt / maxr > 1024)
	{
		c = TERM_SLATE;
		r = "+---rare---+";
	}
	if (maxt / maxr > 32768L)
	{
		c = TERM_L_DARK;
		r = "+--unique--+";
	}

	/* Calculate probabilities for each range */
	for (i = 0; i < 20; i++)
	{
		/* Shift the values into view */
		for (j = i * MAX_DEPTH / 20; j < (i + 1) * MAX_DEPTH / 20; j++)
		{
			display[i] += rarity[j] * maxt * 10 / total[j];
		}

		/* Correct proportions */
		display[i] /= maxr;

		/* Track maximum */
		if (display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	for (i = 0; i < 20; i++)
	{
		display[i] = display[i] * 10 / maxd;
	}

	/* Graph the rarities */
	for (i = 0; i < 20; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE,  '|');

		/* Note the level */
		if ((i * MAX_DEPTH / 20 <= home) && (home < (i + 1) * MAX_DEPTH / 20))
		{
			c_prt(TERM_RED, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
		else
		{
			c_prt(c, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
	}

	/* Make it look nice */
	prt(r, row, col);

	Term_putch(col, row + 2, TERM_WHITE,  '6');

	Term_putch(col, row + 8, TERM_WHITE,  'A');
	Term_putch(col, row + 9, TERM_WHITE,  'L');
	Term_putch(col, row + 10, TERM_WHITE, 'L');
	Term_putch(col, row + 11, TERM_WHITE, 'O');
	Term_putch(col, row + 12, TERM_WHITE, 'C');

	prt("+", row + 21, col);
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!target_who) return;

	/* Teleport to the target */
	teleport_player_to(target_row, target_col);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
static void do_cmd_wiz_change_aux(void)
{
	int i;
	int tmp_int;
	long tmp_long;
	char tmp_val[160];
	char ppp[80];


	/* Query the stats */
	for (i = 0; i < 6; i++)
	{
		/* Prompt */
		sprintf(ppp, "%s (3-118): ", stat_names[i]);

		/* Default */
		sprintf(tmp_val, "%d", p_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 3)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > 18+100) tmp_int = 18+100;
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->max_exp = tmp_long;
	p_ptr->exp = tmp_long;

	/* Update */
	check_experience();
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(void)
{
	/* Interact */
	do_cmd_wiz_change_aux();

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Create a feature near the player.
 */
static void do_cmd_wiz_feature(int feat)
{
	int y, x, d = 3, attempts = 30;

	while (1)
	{
		/* Find a location */
		y = rand_spread(py, d);
		x = rand_spread(px, d);

		/* Reject illegal grids */
		if (!in_bounds(y, x)) continue;

		/* Reject the player */
		if ((y == py) && (x == px)) continue;

		attempts--;

		if (!attempts)
		{
			d++;
			attempts = 8 * d;
		}

		/* Try to place a new feature */
		if (area(y, x)->feat == feat) continue;

		/* Okay */
		break;
	}

	/* Nuke objects */
	delete_object_idx(area(y, x)->o_idx);

	/* Nuke monsters */
	delete_monster_idx(area(y, x)->m_idx);

	/* Forget this grid */
	area(y, x)->info &= ~(CAVE_MARK);

	/* Place the feature */
	cave_set_feat(y, x, feat);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_MONSTERS);
}

/* Learn the whole wilderness map */
static void learn_map(void)
{
	int i, j;
	for (i = 0; i < max_wild; i++)
	{
		for (j = 0; j < max_wild; j++)
		{
			wild[j][i].done.info |= WILD_INFO_SEEN;
		}
	}
}

/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
static void wiz_display_item(object_type *o_ptr)
{
	int i, j = 13;
	u32b f1, f2, f3;
	char buf[256];

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	prt_alloc(o_ptr->tval, o_ptr->sval, 2, 0);

	/* Describe fully */
	object_desc_store(buf, o_ptr, TRUE, 3);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
	           o_ptr->k_idx, get_object_level(o_ptr),
	           o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
	           o_ptr->number, o_ptr->weight,
	           o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
	           o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
	           o_ptr->name1, o_ptr->name2, (long)object_value(o_ptr)), 7, j);

	prt(format("ident = %04x  xtra1 = %-4d  xtra2 = %-4d  timeout = %-d",
	           o_ptr->ident, o_ptr->xtra1, o_ptr->xtra2, o_ptr->timeout), 8, j);

	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT........SLAY........BRAND.", 11, j);
	prt("              cvae      xsqpaefc", 12, j);
	prt("siwdcc  ssidsahanvudotgddhuoclio", 13, j);
	prt("tnieoh  trnipttmiinmrrnrrraiierl", 14, j);
	prt("rtsxna..lcfgdkcpmldncltggpksdced", 15, j);
	prt_binary(f1, 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt("SUST...IMMUN..RESIST............", 18, j);
	prt("        aefctrpsaefcpfldbc sn   ", 19, j);
	prt("siwdcc  clioheatcliooeialoshtncd", 20, j);
	prt("tnieoh  ierlrfraierliatrnnnrhehi", 21, j);
	prt("rtsxna..dcedwlatdcedsrekdfddrxss", 22, j);
	prt_binary(f2, 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt("fe      ehsi  st    iiiiadta  hp", 11, j+32);
	prt("il   n taihnf ee    ggggcregb vr", 12, j+32);
	prt("re  no ysdose eld   nnnntalrl ym", 13, j+32);
	prt("ec  om cyewta ieirmsrrrriieaeccc", 14, j+32);
	prt("aa  ta uktmatlnpgeihaefcvnpvsuuu", 15, j+32);
	prt("uu  eg rnyoahivaeggoclioaeoasrrr", 16, j+32);
	prt("rr  li sopdretitsehtierltxrtesss", 17, j+32);
	prt("aa  ec ewestreshtntsdcedeptedeee", 18, j+32);
	prt_binary(f3, 19, j+32);
}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static tval_desc tvals[] =
{
	{ TV_SWORD,             "Sword"                },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_HAFTED,            "Hafted Weapon"        },
	{ TV_BOW,               "Bow"                  },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_SHOT,              "Shots"                },
	{ TV_SHIELD,            "Shield"               },
	{ TV_CROWN,             "Crown"                },
	{ TV_HELM,              "Helm"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR,        "Hard Armor"           },
	{ TV_SOFT_ARMOR,        "Soft Armor"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
	{ TV_LIFE_BOOK,         "Life Spellbook"       },
	{ TV_SORCERY_BOOK,      "Sorcery Spellbook"    },
	{ TV_NATURE_BOOK,       "Nature Spellbook"     },
	{ TV_CHAOS_BOOK,        "Chaos Spellbook"      },
	{ TV_DEATH_BOOK,        "Death Spellbook"      },
	{ TV_TRUMP_BOOK,        "Trump Spellbook"      },
	{ TV_ARCANE_BOOK,       "Arcane Spellbook"     },
	{ TV_SPIKE,             "Spikes"               },
	{ TV_DIGGING,           "Digger"               },
	{ TV_CHEST,             "Chest"                },
	{ TV_FIGURINE,          "Magical Figurine"     },
	{ TV_STATUE,            "Statue"               },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask"                },
	{ TV_JUNK,              "Junk"                 },
	{ TV_SKELETON,          "Skeleton"             },
	{ 0,                    NULL                   }
};


/*
 * Strip an "object name" into a buffer
 */
static void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


/*
 * Specify tval and sval (type and subtype of object) originally
 * by RAK, heavily modified by -Bernd-
 *
 * This function returns the k_idx of an object type, or zero if failed
 *
 * List up to 50 choices in three columns
 */
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	int tval;

	cptr tval_desc;
	char ch;

	int choice[60];

	char buf[160];


	/* Clear screen */
	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 60) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 30 * (num / 20);
		ch = listsym[num];
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* Me need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if (!get_com("Get what type of object? ", &ch)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is illegal */
	if (num >= max_num) return (0);

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 60) && (i < max_k_idx); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
		{
			/* Prepare it */
			row = 2 + (num % 20);
			col = 30 * (num / 20);
			ch = listsym[num];

			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is "illegal" */
	if (num >= max_num) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];


	p = "Enter new 'pval' setting: ";
	sprintf(tmp_val, "%d", o_ptr->pval);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'xtra2' setting: ";
	sprintf(tmp_val, "%d", o_ptr->xtra2);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->xtra2 = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static void wiz_reroll_item(object_type *o_ptr)
{
	object_type forge;
	object_type *q_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;


	/* Get local object */
	q_ptr = &forge;

	/* Copy the object */
	object_copy(q_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(q_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [w]orthless, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch))
		{
			/* Preserve wizard-generated artifacts */
			if (artifact_p(q_ptr))
			{
				a_info[q_ptr->name1].cur_num = 0;
				q_ptr->name1 = 0;
			}

			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Preserve wizard-generated artifacts */
		if (artifact_p(q_ptr))
		{
			a_info[q_ptr->name1].cur_num = 0;
			q_ptr->name1 = 0;
		}

		switch(ch)
		{
			/* Apply bad magic, but first clear object */
			case 'w': case 'W':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, TRUE, TRUE);
				break;
			}
			/* Apply bad magic, but first clear object */
			case 'c': case 'C':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, FALSE, TRUE);
				break;
			}
			/* Apply normal magic, but first clear object */
			case 'n': case 'N':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE, FALSE);
				break;
			}
			/* Apply good magic, but first clear object */
			case 'g': case 'G':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, FALSE, FALSE);
				break;
			}
			/* Apply great magic, but first clear object */
			case 'e': case 'E':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, TRUE, FALSE);
				break;
			}
			case 's': case 'S':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, TRUE, TRUE, TRUE, FALSE);

				/* Failed to create normal artifact; make a random one */
				if (!artifact_p(q_ptr)) create_artifact(q_ptr, FALSE);
				break;
			}
		}
	}


	/* Notice change */
	if (changed)
	{
		/* Restore the position information */
		q_ptr->iy = o_ptr->iy;
		q_ptr->ix = o_ptr->ix;
		q_ptr->next_o_idx = o_ptr->next_o_idx;
		q_ptr->marked = o_ptr->marked;

		/* Apply changes */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}
}



/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
static void wiz_statistics(object_type *o_ptr)
{
	u32b i, matches, better, worse, other, correct;

	u32b test_roll = 1000000;

	char ch;
	char *quality;

	bool good, great;

	object_type forge;
	object_type	*q_ptr;

	cptr q = "Rolls: %ld  Correct: %ld  Matches: %ld  Better: %ld  Worse: %ld  Other: %ld";

	cptr p = "Enter number of items to roll: ";
	char tmp_val[80];


	/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch)) break;

		if (ch == 'n' || ch == 'N')
		{
			good = FALSE;
			great = FALSE;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			good = TRUE;
			great = FALSE;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			good = TRUE;
			great = TRUE;
			quality = "excellent";
		}
		else
		{
			good = FALSE;
			great = FALSE;
			break;
		}

		sprintf(tmp_val, "%ld", test_roll);
		if (get_string(p, tmp_val, 10)) test_roll = atol(tmp_val);
		test_roll = MAX(1, test_roll);

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
					  quality, dun_level);
		msg_print(NULL);

		/* Set counters to zero */
		correct = matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= test_roll; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, correct, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Create an object */
			make_object(q_ptr, good, great);


			/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
			if (artifact_p(q_ptr)) a_info[q_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (q_ptr->tval)) continue;
			if ((o_ptr->sval) != (q_ptr->sval)) continue;

			/* One more correct item */
			correct++;

			/* Check for match */
			if ((q_ptr->pval == o_ptr->pval) &&
				 (q_ptr->to_a == o_ptr->to_a) &&
				 (q_ptr->to_h == o_ptr->to_h) &&
				 (q_ptr->to_d == o_ptr->to_d) &&
				 (q_ptr->name1 == o_ptr->name1))
			{
				matches++;
			}

			/* Check for better */
			else if ((q_ptr->pval >= o_ptr->pval) &&
						(q_ptr->to_a >= o_ptr->to_a) &&
						(q_ptr->to_h >= o_ptr->to_h) &&
						(q_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((q_ptr->pval <= o_ptr->pval) &&
						(q_ptr->to_a <= o_ptr->to_a) &&
						(q_ptr->to_h <= o_ptr->to_h) &&
						(q_ptr->to_d <= o_ptr->to_d))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, correct, matches, better, worse, other);
		msg_print(NULL);
	}


	/* Hack -- Normally only make a single artifact */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *o_ptr)
{
	int         tmp_int, tmp_qnt;

	char        tmp_val[100];


	/* Never duplicate artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;

	/* Store old quantity. -LM- */
	tmp_qnt = o_ptr->number;

	/* Default */
	sprintf(tmp_val, "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 2))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;
		
		/* Add the weight */
		p_ptr->total_weight += ((tmp_int - o_ptr->number) * o_ptr->weight);

		/* Accept modifications */
		o_ptr->number = tmp_int;

		/* Hack -- rod pvals must change if the number in the stack does. -LM- */
		if (o_ptr->tval == TV_ROD)
			o_ptr->pval = o_ptr->pval * o_ptr->number / tmp_qnt;
	}
}



/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(void)
{
	int item;

	object_type	forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	bool changed;

	cptr q, s;

	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* The item was not changed */
	changed = FALSE;


	/* Save the screen */
	screen_save();


	/* Get local object */
	q_ptr = &forge;

	/* Copy object */
	object_copy(q_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(q_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch))
		{
			changed = FALSE;
			break;
		}

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(q_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			wiz_reroll_item(q_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(q_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(q_ptr);
		}
	}


	/* Restore the screen */
	screen_load();


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Change */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
static void wiz_create_item(void)
{
	object_type	forge;
	object_type *q_ptr;

	int k_idx;


	/* Save the screen */
	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	screen_load();


	/* Return if failed */
	if (!k_idx) return;

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
	object_prep(q_ptr, k_idx);

	if (k_info[k_idx].flags3 & TR3_INSTA_ART)
	{
		int i;

		/* Artifactify */
		for (i = 1; i < max_a_idx; i++)
		{
			/* Ignore incorrect tval */
			if (a_info[i].tval != q_ptr->tval) continue;

			/* Ignore incorrect sval */
			if (a_info[i].sval != q_ptr->sval) continue;

			/* Choose this artifact */
			q_ptr->name1 = i;
			break;
		}

		/* Apply magic */
		apply_magic(q_ptr, -1, TRUE, TRUE, TRUE, FALSE);
	}
	else
	{
		/* Apply magic */
		apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE, FALSE);
	}

#ifdef USE_SCRIPT
	q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

	/* Drop the object from heaven */
	(void)drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
	/* Remove curses */
	(void)remove_all_curse();

	/* Restore stats */
	(void)res_stat(A_STR);
	(void)res_stat(A_INT);
	(void)res_stat(A_WIS);
	(void)res_stat(A_CON);
	(void)res_stat(A_DEX);
	(void)res_stat(A_CHR);

	/* Restore the level */
	(void)restore_level();

	/* Heal the player */
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;

	/* Restore mana */
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

	/* Cure stuff */
	(void)set_blind(0);
	(void)set_confused(0);
	(void)set_poisoned(0);
	(void)set_afraid(0);
	(void)set_paralyzed(0);
	(void)set_image(0);
	(void)set_stun(0);
	(void)set_cut(0);
	(void)set_slow(0);

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(void)
{
	/* Ask for level */
	if (command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];

		/* Prompt */
		sprintf(ppp, "Jump to level (0-%d): ", MAX_DEPTH-1);

		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}

	/* Paranoia */
	if (command_arg < 0) command_arg = 0;

	/* Paranoia */
	if (command_arg > MAX_DEPTH - 1) command_arg = MAX_DEPTH - 1;

	/* Accept request */
	msg_format("You jump to dungeon level %d.", command_arg);

	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	p_ptr->inside_arena = 0;
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
		(quest[leaving_quest].flags & QUEST_FLAG_ONCE) &&
		(quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
	}

	p_ptr->inside_quest = 0;
	p_ptr->leftbldg = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= command_arg)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare object */
			object_prep(q_ptr, i);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
static void do_cmd_wiz_summon(int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		(void)summon_specific(0, py, px, dun_level, 0, TRUE, FALSE, FALSE);
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(int r_idx, bool slp)
{
	int i, x, y;

	cave_type *c_ptr;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx) return;

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, py, px, d, 0);

		/* paranoia */
		if (!in_bounds2(y, x)) continue;

		/* Require empty grids */
		c_ptr = area(y, x);
		if (!cave_empty_grid(c_ptr)) continue;

		/* Place it (allow groups) */
		if (place_monster_aux(y, x, r_idx, slp, TRUE, FALSE, FALSE)) break;
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(int r_idx, bool slp)
{
	(void)summon_named_creature(py, px, r_idx, slp, TRUE, TRUE);
}


/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(void)
{
	int i;


	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT) delete_monster_idx(i);
	}
}


/*
 * Hack -- Delete all monsters
 */
static void do_cmd_wiz_zap_all(void)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete this monster */
		delete_monster_idx(i);
	}
}


#ifdef USE_SCRIPT

/*
 * Hack -- Execute a script function
 */
static void do_cmd_wiz_script(void)
{
	int err;
	char name[80];

	/* Get name of script to execute */
	name[0] = '\0';

	if (!get_string("Function name: ", name, 80)) return;

	/* No name, no execute */
	if (name[0] == '\0')
	{
		msg_print("Cancelled.");
		return;
	}

	/* Execute script */
	err = script_execute(name);

	/* Error */
	if (err)
	{
		msg_print("Failed.");
		return;
	}
}

#endif /* USE_SCRIPT */



#ifdef ALLOW_SPOILERS

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(void);


/*
 * The following code fragment outputs a matlab
 * 'm' file that can be used to balance the
 * distribution of monsters in the dungeon
 */
#ifdef MATLAB


/*
 * This function outputs the values of a flag
 * to the m file
 */
static void outflagmatlab(u32b flag, FILE *fff)
{
	int i, j;

	char buf[3];

	i = 1;
	for (j = 0; j < 32; j++, i<<=1)
	{
		/* If the bit is set */
		if (flag & i)
		{
			sprintf(buf, "1 ");
		}
		else
		{
			sprintf(buf, "0 ");
		}

		fprintf(fff, buf);
	}
	fprintf(fff, "...\n");
}

/*
 * This function outputs the values of a flag.
 * However, the flag is scaled by a parameter in
 * some cases.
 * MEGA HACK - on every bit except those in first
 * byte.  (to match with the monster breaths)
 * This is wrong with the other flags... but it seems
 * to produce a fairly good fit to the data anyway.
 */
static void outflagmatlab2(u32b flag, FILE *fff, int hp)
{
	int i, j;

	char buf[10];

	i = 1;
	for (j = 0; j < 32; j++, i<<=1)
	{
		/* If the bit is set */
		if (flag & i)
		{
			if (i < 256)
			{
				sprintf(buf, "1 ");
			}
			else
			{
				/* A breath - scale it by the hp */
				sprintf(buf, "%d ", hp);
			}
		}
		else
		{
			sprintf(buf, "0 ");
		}

		fprintf(fff, buf);
	}
	fprintf(fff, "...\n");
}


/*
 * This function outputs the blows table for a monster.
 * The results are scaled by the damage x2.
 */
static void outblowsmatlab(monster_race *r_ptr, char *buf2)
{
	int i, effect;
	int effects[32];

	/* Clear effects list */
	for (i = 0; i < 32; i++)
	{
		effects[i] = 0;
	}

	/* For each blow */
	for (i = 0; i < 4; i++)
	{
		/* Only count real blows */
		if (!r_ptr->blow[i].method) continue;

		/* damage x2 */
		effect = r_ptr->blow[i].d_dice * (r_ptr->blow[i].d_side + 1);

		/* Count "weird" effects */
		if (effect == 0) effect = 1;

		effects[r_ptr->blow[i].effect] += effect;
	}

	/* Output effects list */
	for (i = 0; i < 32; i++)
	{
		sprintf(buf2, "%s %d", buf2, effects[i]);
	}
}


/*
 * This function returns the "power" of a monster
 * existing on a level.
 * This function was calibrated by running several
 * warriors and mages to stat depth an beyond by John H.
 *
 * clevel 0->35   dlevel 0->40
 *       35->50         40->70
 *       50->55*        70->100
 * *Note 55 because the player finds artifacts.
 * This seems to work well.  The inhomogeneous distribution
 * of new monsters / level is made nice and flat.
 */
static int monster_power_mat(int level)
{
	if (level < 40) return (35 * level / 4);
	if (level < 70) return (5 * (level - 40) + 350);
	return (((level - 70) * 5) / 3 + 500);
}

/*
 * This function outputs a MATLAB 'm'file that can
 * be used to balance the monster distribution in the
 * game.
 *
 * Run this function, then do the following:
 * 1) copy monmatlb.m from the /lib/save directory to
 * 	a directory in the matlab path.
 * 2) type "monmatlb" in MATLAB.
 * 3) The matrix is called xx, the monster depths are y,
 *	the monster xp values are xp.
 * 4) Now the balanced values are calculated:
 * 5) Type: "a1=xx\y';"
 * 6) Type: "r1=xx*a1;"
 * 7) Type: "level=r1*100/550;"
 * 8) Type: "i=20:884;"  (Note these are the values of min
 *	and max used below in the code.
 * 9) Type: "[i' round(level)]" MATLAB should now print out a table
 *	of monster number vs new monster depth.
 * 10) To work out the new xp values:
 * 11) Type: "xp2=xp.^(0.15);"  This function scales the xp so
 *	that it seems to be linear with the new monster distribution.
 * 12) Type: "a2=xx\xp2';"
 * 13) Type: "r2=xx*a2;"
 * 14) Type: "xp3=round(r2.^(1/0.15))" MATLAB should now print a list
 *	of the new xp values... use your favourite editor to move this
 *	list next to the old one.  NOTE: My version of matlab cannot
 *	print out the monster number next to the xp without "running
 *	off the top of the screen" so that you cannot copy the result.
 */
void output_monster_matlab(void)
{
	int i, max, min;
	monster_race *r_ptr;
	char buf[2048], buf2[50];
	unsigned int hp;
	FILE *fff;

	/* Create the file name */
	path_build(buf, 500, ANGBAND_DIR_SAVE, "monmatlb.m");

	/* Drop priv's */
	safe_setuid_drop();

	/* Open file */
	fff = my_fopen(buf, "w");

	/* Grab priv's */
	safe_setuid_grab();

	/* Failure */
	if (!fff) return;

	/* Add starting stuff */
	fprintf(fff, "xx=[];\n");

	/* Min and max monsters to output in the matrix */
	max = max_r_idx;
	min = 20;

	for (i = min; i < max; i++)
	{
		fprintf(fff, "x=[");

		/* Get race */
		r_ptr =  &r_info[i];

		/* Hitpoints x2 */
		hp = r_ptr->hdice * (r_ptr->hside + 1);
		if (r_ptr->flags1 & RF1_FORCE_MAXHP)
		{
			/* hp x2 */
			hp = r_ptr->hdice * r_ptr->hside * 2;
		}

		/* Output the flags one by one */
		outflagmatlab(r_ptr->flags1 & ~(0x00000020F), fff);
		outflagmatlab(r_ptr->flags2, fff);
		outflagmatlab(r_ptr->flags3, fff);
		outflagmatlab2(r_ptr->flags4, fff, hp);
		outflagmatlab2(r_ptr->flags5, fff, r_ptr->level);
		outflagmatlab2(r_ptr->flags6, fff, r_ptr->level);
		outflagmatlab2(r_ptr->flags8, fff, r_ptr->level);

		/* Numerical flags */
		sprintf(buf2, "%d", r_ptr->speed);

		sprintf(buf2, "%s %d", buf2, hp);
		sprintf(buf2, "%s %d", buf2, r_ptr->aaf);
		sprintf(buf2, "%s %d", buf2, r_ptr->ac);
		sprintf(buf2, "%s %d", buf2, r_ptr->sleep);
		sprintf(buf2, "%s %d", buf2, r_ptr->freq_inate + r_ptr->freq_spell);

		/* The blows table */
		outblowsmatlab(r_ptr, buf2);
		sprintf(buf2, "%s];\nxx=[xx;x];\n", buf2);

		fprintf(fff, buf2);
	}

	/* Output the power information */
	fprintf(fff, "y=[");

	for (i = min; i < max; i++)
	{
		/* Get race */
		r_ptr =  &r_info[i];

		/* Output the level */
		sprintf(buf2, "%d ",  monster_power_mat(r_ptr->level));

		fprintf(fff, buf2);
	}

	fprintf(fff, "];\n");

	/* Output the XP information */
	fprintf(fff, "xp=[");

	for (i = min; i < max; i++)
	{
		/* Get race */
		r_ptr =  &r_info[i];

		/* Output the experience for a kill */
		sprintf(buf2, "%ld ", r_ptr->mexp);

		fprintf(fff, buf2);
	}

	fprintf(fff, "];");

	my_fclose(fff);
}
#endif /* MATLAB */

/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(void)
{
	int     x, y;
	char    cmd;


	/* Get a "debug command" */
	get_com("Debug Command: ", &cmd);

	/* Analyze the command */
	switch (cmd)
	{
		/* Nothing */
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		break;


#ifdef ALLOW_SPOILERS

		/* Hack -- Generate Spoilers */
		case '"':
		do_cmd_spoilers();
		break;

#endif /* ALLOW_SPOILERS */

#ifdef MATLAB
		case '=':
		output_monster_matlab();
		break;
#endif /* MATLAB */

		/* Hack -- Help */
		case '?':
		do_cmd_help();
		break;


		/* Cure all maladies */
		case 'a':
		do_cmd_wiz_cure_all();
		break;

		/* Know alignment */
		case 'A':
		msg_format("Your alignment is %d.", p_ptr->align);
		break;

		/* Teleport to target */
		case 'b':
		do_cmd_wiz_bamf();
		break;

		/* Create any object */
		case 'c':
		wiz_create_item();
		break;

		/* Create a named artifact */
		case 'C':
		wiz_create_named_art(command_arg);
		break;

		/* Detect everything */
		case 'd':
		detect_all();
		break;

		/* Edit character */
		case 'e':
		do_cmd_wiz_change();
		break;

		/* View item info */
		case 'f':
		identify_fully();
		break;

		/* Create feature */
		case 'F':
		if (command_arg > 0) do_cmd_wiz_feature(command_arg);
		break;

		/* Good Objects */
		case 'g':
		if (command_arg <= 0) command_arg = 1;
		acquirement(py, px, command_arg, FALSE, TRUE);
		break;

		/* Hitpoint rerating */
		case 'h':
		do_cmd_rerate(); break;

#ifdef MONSTER_HORDES
		case 'H':
		do_cmd_summon_horde(); break;
#endif /* MONSTER_HORDES */

		/* Identify */
		case 'i':
		(void)ident_spell();
		break;

		/* Fields Integrity */
		case 'I':
		(void)test_field_data_integrity();
		break;

		/* Go up or down in the dungeon */
		case 'j':
		do_cmd_wiz_jump();
		break;

		/* Self-Knowledge */
		case 'k':
			self_knowledge();
			break;

		/* Learn about objects */
		case 'l':
			do_cmd_wiz_learn();
			break;

		/* Magic Mapping */
		case 'm':
			map_area();
			break;

		/* Mutation */
		case 'M':
			(void)gain_random_mutation(command_arg);
			break;

		/* Specific reward */
		case 'r':
			(void)gain_level_reward(command_arg);
			break;

		/* Summon _friendly_ named monster */
		case 'N':
			do_cmd_wiz_named_friendly(command_arg, TRUE);
			break;

		/* Summon Named Monster */
		case 'n':
			do_cmd_wiz_named(command_arg, TRUE);
			break;

		/* Object playing routines */
		case 'o':
			do_cmd_wiz_play();
			break;

		/* Phase Door */
		case 'p':
			teleport_player(10);
			break;

#if 0
		/* Complete a Quest -KMW- */
		case 'q':
		{
			for (i = 0; i < max_quests; i++)
			{
				if (p_ptr->quest[i].status == QUEST_STATUS_TAKEN)
				{
					p_ptr->quest[i].status++;
					msg_print("Completed Quest");
					msg_print(NULL);
					break;
				}
			}
			if (i == max_quests)
			{
				msg_print("No current quest");
				msg_print(NULL);
			}
			break;
		}
#endif

		/* Make every dungeon square "known" to test streamers -KMW- */
		case 'u':
		{
			for (y = min_hgt; y < max_hgt; y++)
			{
				for (x = min_wid; x < max_wid; x++)
				{
					area(y, x)->info |= (CAVE_GLOW | CAVE_MARK);
				}
			}

			wiz_lite();
			break;
		}

		/* Summon Random Monster(s) */
		case 's':
			if (command_arg <= 0) command_arg = 1;
			do_cmd_wiz_summon(command_arg);
			break;

		/* Teleport */
		case 't':
			teleport_player(100);
			break;

		/* Very Good Objects */
		case 'v':
			if (command_arg <= 0) command_arg = 1;
			acquirement(py, px, command_arg, TRUE, TRUE);
			break;

		/* Wizard Light the Level */
		case 'w':
			if (dun_level)
			{
				wiz_lite();
			}
			else
			{
				learn_map();
			}
		break;

		case 'W':
		test_decision_tree();
		break;

		/* Increase Experience */
		case 'x':
		if (command_arg)
		{
			gain_exp(command_arg);
		}
		else
		{
			gain_exp(p_ptr->exp + 1);
		}
		break;

		/* Zap Monsters (Genocide) */
		case 'z':
		do_cmd_wiz_zap();
		break;

		case 'Z':
		do_cmd_wiz_zap_all();
		break;

		/* Hack -- whatever I desire */
		case '_':
		do_cmd_wiz_hack_ben();
		break;

#ifdef USE_SCRIPT
		/* Hack -- activate a script */
		case '@':
		do_cmd_wiz_script();
		break;
#endif /* USE_SCRIPT */

		/* Not a Wizard Command */
		default:
		msg_print("That is not a valid debug command.");
		break;
	}
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif
