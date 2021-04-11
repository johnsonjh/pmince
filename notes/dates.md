Keeping in mind that I'm only an *accidental* "expert" (thus, not *actually* an expert at all) regarding **MINCE**, the following should be considered facts as they reference **verifiable** sources:

## MINCE 1: 1979

- **MINCE** (version 1) was functional by [late 1979](https://en.wikipedia.org/wiki/Talk:Sprint_(word_processor)#3_Based_on_MINCE?). 
  - There only a few **tiny** fragments of pre-1980 **MINCE** code available, recovered from the "unused" areas of old disks.

  - While I do not (yet) have a more specific date for **MINCE** 1, it would have to be after the 1979 release of [CP/M 2.0](http://bitsavers.org/pdf/digitalResearch/cpm/2.0) and the August 1979 release of [BDS C](https://www.bdsoft.com/resources/bdsc.html).

## MINCE 2: 1980

- **MINCE** (version 2) saw commercial distribution in [September 1980](https://raw.githubusercontent.com/johnsonjh/pmince/master/legal/73576514.pdf); the earliest (*reliable*) dates for the creation of version 2 are [May, 1980](https://github.com/johnsonjh/pmince/blob/master/oemkit/mince/80/term.c).

  - Other dates in the source code are ambiguous but seem to indicate version 1 being complete by January 1980.

## Users Group

* The [AMETHYST-USERS mailing list](https://github.com/johnsonjh/pmince/tree/master/3rdparty/aug/mail) was on MIT-MC, as were the [AUG](https://github.com/johnsonjh/pmince/tree/master/3rdparty/aug/files) archives. I don't have much details from the "1st generation" AUG, but the "second generation" AUG was resurrected in the summer of 1982.

  * [AMETHYST](https://github.com/johnsonjh/pmince/blob/master/3rdparty/media/MINCEAD1.png) refers to a bundling of **MINCE**, [SCRIBBLE](https://github.com/johnsonjh/pmince/tree/master/oemkit/scribble/80), and BDS C.
```text
       13-Jul-82 23:55:00,984;000000000000
       Date:     14 July 1982 0155-edt
       From:     Brian N. Hess              <Hess.Unicorn at MIT-MULTICS>
       Subject:  Amethyst Users Group
       To:       Amethyst-Users at MIT-AI
       
       Quite a few people have asked about what ever happened to the Amethyst
       Users Group and the AUGment newsletter.  Well, after a long hiatus, they
       are about to be resurrected again.  For the time being, you can address
       AUG mail to Mark of the Unicorn rather than to the Texas address which
       you may have come to know and love.  Since the group should really be
       run by someone who's not a MotU person, are there any volunteers out
       there who have modified Mince a lot and have free time to devote to this?
       We're looking for a replacement for me...  If you've written to the
       Texas address, patience is the keyword -- we're going to revive the
       group somehow, but how quickly or what it will look like when
       reincarnated we don't know!
                                     Cheers,
                                     Brian
```
* As can be seen above, *MOTU* was assigned the "**Unicorn**" project on *MIT-MULTICS* (since at least mid-1982) - thus, the similarities between **MINCE** and Multics Emacs (*in my opinion*) cannot be accidental.
 
* In fact, **MINCE** users specifically noted the [similarity to Multics Emacs](https://github.com/johnsonjh/pmince/blob/master/3rdparty/aug/mail/8207-1.txt) as a major feature.

## Post-MINCE

* **MINCE** became the core of *Perfect* *Writer*. Indeed, *PW* was **MINCE** with many of the AUG enhancements built-in - the compiled editor core is about 6K larger than stock **MINCE** 2.6 on an 8080.  *Perfect* *Writer* was further enhanced to become *Final* *Word*.  *FW* was later purchased by Borland and became [Borland Sprint](https://en.wikipedia.org/wiki/Sprint_(word_processor)).

## Notable Users at MIT

*  MINCE/PW was in wide-use by the MIT community; Marvin Minsky [used it](https://github.com/johnsonjh/pmince/blob/master/3rdparty/aug/mail/9101-1.txt) to write "*The Society of Mind*".

## Further Information

- Everything that I know about MINCE can be found in the [Portable MINCE](https://github.com/johnsonjh/pmince) repository.

---

A bit more info … DDJ and era interviews indicate that BDS C was working by January 1979 (at least in some state), and released around April 1979, earlier than previously mentioned above.

Leor Zolman was from MIT, the Unicorn guys were from MIT, and articles from this time indicate that others, all (obviously) connected with MIT as well, had been using the BDS C software before it was generally released as a product.

Indeed, an 01/1980 DDJ article mentions BDS C was available at that time, and that the Tiny-C people had been using it with success for quite awhile before that date.

A contemporary interview with Leor Zolman lines up, as he states he knew the Tiny-C people well (but was not aware of Small-C at the time.)

I'm considering writing him to ask if he might recall when he provided his software to the Unicorn people!

I normally wouldn't bother someone for just this kind of question, especially since BDS C history is well documented - and I sure wouldn't expect anyone to remember a sale from 42 years before, but, in this case, it's likely that the Unicorn people must have been among the very first users.

Also, the fact that the MOTU AMETHYST product actually bundled BDS C points to a special relationship, so it's likely worth asking. I'd not be surprised if Mark of the Unicorn was the very first reseller of BDS C.

So, it is a *possibility* that the first MINCE could have been any time after 01/79.

Now, I looked at many DDJ and BYTE scans, as well as the CP/M 2 source code comments, and can date the release of CP/M 2 to August 1979.

So, while it's possible that MINCE was begun earlier and adapted for CP/M 2, looking at the code makes me think it was not. Judgement call here too, but I've looked at **a lot** of CP/M code lately! 

This all also lines up with the "late 1979" recollection linked above.

Now, until definitive answers come back, this is nothing more than a somewhat well informed guess... but, based on the rate of development (as evidenced by the comments) and, assuming that work progressed more quickly at first than later on (...well known fact that fixing bugs and polishing a release takes longer than getting the core functionality done ... the 80:20 rule ... etc) ... I'd conclude, with high confidence, that the first workable version of MINCE must be dated to sometime between September and December of 1979.
