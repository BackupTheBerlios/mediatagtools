#ifndef MP3EXTRAFRAMES_H
#define MP3EXTRAFRAMES_H

#define EF_NUM 41

static const char* extraFrames[EF_NUM][2] = {   /*{ "TALB", "Album/Movie/Show title" },*/
                                            { "TBPM", "BPM (beats per minute)" },
                                            { "TCOM", "Composer" },
//                                             { "TCON", "Content type" },
                                            { "TCOP", "Copyright message" },
                                            { "TDEN", "Encoding time" },
                                            { "TDLY", "Playlist delay" },
                                            { "TDOR", "Original release time" },
                                            { "TDRC", "Recording time" },
                                            { "TDRL", "Release time" },
                                            { "TDTG", "Tagging time" },
                                            { "TENC", "Encoded by" },
                                            { "TEXT", "Lyricist/Text writer" },
                                            { "TFLT", "File type" },
                                            { "TIPL", "Involved people list" },
                                            { "TIT1", "Content group description" },
//                                             { "TIT2", "Title/songname/content description" },
                                            { "TIT3", "Subtitle/Description refinement" },
                                            { "TKEY", "Initial key" },
                                            { "TLAN", "Language(s)" },
                                            { "TLEN", "Length" },
                                            { "TMCL", "Musician credits list" },
                                            { "TMED", "Media type" },
                                            { "TMOO", "Mood" },
                                            { "TOAL", "Original album/movie/show title" },
                                            { "TOFN", "Original filename" },
                                            { "TOLY", "Original lyricist(s)/text writer(s)" },
                                            { "TOPE", "Original artist(s)/performer(s)" },
                                            { "TOWN", "File owner/licensee" },
//                                             { "TPE1", "Lead performer(s)/Soloist(s)" },
                                            { "TPE2", "Band/orchestra/accompaniment" },
                                            { "TPE3", "Conductor/performer refinement" },
                                            { "TPE4", "Interpreted, remixed, or otherwise modified by" },
                                            { "TPOS", "Part of a set" },
                                            { "TPRO", "Produced notice" },
                                            { "TPUB", "Publisher" },
//                                             { "TRCK", "Track number/Position in set" },
                                            { "TRSN", "Internet radio station name" },
                                            { "TRSO", "Internet radio station owner" },
                                            { "TSOA", "Album sort order" },
                                            { "TSOP", "Performer sort order" },
                                            { "TSOT", "Title sort order" },
                                            { "TSRC", "ISRC (international standard recording code)" },
                                            { "TSSE", "Software/Hardware and settings used for encoding" },
                                            { "TSST", "Set subtitle" },
					    { "UFID", "Unique file identifier" } };

#endif
