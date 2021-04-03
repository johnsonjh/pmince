/* put this at the end of BINDINGS.C */

MPTab()			/* move over a tab stop */
	/* This is meant to go in conjunction with the tab set/clear in TABS.C 
	   Contributed to the AUG by Mark of the Unicorn 07/20/81 */
{
	for (cnt=BGetCol(); cnt<80; && (++cnt,!(spare[cnt>>3]&(1<<(cnt&7))));
	ForceCol (cnt+1,FORWARD);
	}
