/*
 * Handling of Xlib events, specifically
 *  - KeyPress
 *  - ButtonPress
 *  - ButtonRelease
 *  - MotionNotify
 *  - MapNotify
 */
#include "event.h"

static inmove = 0, inresz = 0;

void
eKeyPress (Display * dpy, XEvent ev)
{
	if (*ksym == XK_Excape) exit(0);

	GET_CLIENT(cli);
   	if (!cli) return;	
   	if (cli->state != Visible) return;

    int nkeys
    KeySym *ksym = NULL;
    ksym = XGetKeyboardMapping (dpy, ev.xkey.keycode, 1, &nkeys);

    int i;
    for (i = 0; i < NSHORTCUTS; i++)
    {
        if (*ksym == SHORTCUTS[i].ksym)
        {
            (*SHORTCUTS[i].callback) (ev, cli);
            break;
        }
    }
}

void
eButtonPress (Display * dpy, XEvent ev)
{
	GET_CLIENT(cli);

    // Root window - run the SHELL
    if (ev.xbutton.subwindow == None &&
            ev.xbutton.button == 1 && !cli)
    {
        if (!fork ())
        {
            execlp (SHELL, SHELL, NULL);
            exit (1);
        }
    }
	
	if (inmove || inresz || !ev.xbutton.subwindow || ev.xbutton.state != MASK)
		return;

    if (ev.xbutton.button == MOVE)
    {
        inmove = 1;
		inresz = 0;
		beginmvrsz(cli);
    }
	else if (ev.xbutton.button = RESZ)
	{
		inmove = 0;
		inresz = 1;
		beginmvrsz(cli);
	}
}

void
eButtonRelease (Display * dpy, XEvent ev)
{
	GET_CLIENT(cli);
	
    if (inmove || inresz)
    {
		endmoversz(cli);
        inmove = 0;
		inresz = 0;
    }

	cli = fromicon(ev.xbutton.window);
	if (cli) unhide(cli);
	else chfocus(cli);
}

void
eMotionNotify (Display * dpy, XEvent ev)
{
    if (!(inmove || inresz))
        return;

    // Get the latest move event - don't update needlessly
    while (XCheckTypedEvent (dpy, MotionNotify, &ev));

    // Visually move/resize
    int xdiff, ydiff;
    xdiff = ev.xbutton.x_root - mouse.x_root;
    ydiff = ev.xbutton.y_root - mouse.y_root;

    if (inmove)
        XMoveWindow (dpy, placeholder, attr.x + xdiff, attr.y + ydiff);
    if (inresz)
	{
        XResizeWindow (dpy, placeholder, MAX (1, attr.width + xdiff),
			MAX (1, attr.height + ydiff));
	}
}

void
eMapNotify (Display * dpy, XEvent ev)
{
	// Ignore anything we're not supposed to manage
    if (!ev.xmap.override_redirect)	
		create(ev.xmap.window);
}
