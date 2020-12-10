#!/usr/bin/python
######################################################################
#                                                                    #
#  arraydraw.py: Generates image files from StochSim's array output  #
#                                                                    #
#  Copyright (C) 2001 Tom Shimizu                                    #
#                                                                    #
######################################################################

import getopt, os, sys, string, math
import Image, ImageDraw

linecolour = 0

class ComplexArray:
    def __init__(self, xy, arrayvalues, edgelen):
	self.dimensions = xy
	self.edgelen = edgelen
	self.fillcolour = (255,255,255)
	self.linecolour = (191,191,191)
	self.bgcolour = 254 # (127,127,127)
	self.padding = edgelen
	self.celldim = {'w': 0,'h': 0}
	self.values = arrayvalues
	def tile(self):
	    print "This tile() method shouldn't be called!!\n"

	def _drawcell(self, x, y, value, mode=""):
	    print "This _drawcell() method shouldn't be called!!\n"

class LineArray(ComplexArray):
    def __init__(self, xy, arrayvalues, edgelen):
	ComplexArray.__init__(self, xy, arrayvalues, edgelen)
	self.celldim['w'] = edgelen
	self.celldim['h'] = edgelen
	image_width = self.celldim['w'] * self.dimensions[0] + self.padding * 2
	image_height = self.celldim['h'] * dimensions[1] + self.padding * 2
	self.im = Image.new("L", (image_width,image_height), self.bgcolour)
	self.draw = ImageDraw.Draw(self.im)

    def tile(self):
	for j in range(self.dimensions[1]):
	    x = self.padding
	    y = j * self.celldim['h'] + self.padding
	    for i in range (self.dimensions[0]):
		if (i != 0):
		    x = x + self.celldim['w']
		self._drawcell(x, y, self.values[(i,j)])

    def _drawcell(self, x, y, value):
	colour = value # (value, value, value)
	if drawline:
	    outlinecolour = linecolour
	else:
	    outlinecolour = colour
	vertex1 = (x, y)
	vertex2 = (x + self.celldim['w'], y)
	vertex3 = (x + self.celldim['w'], y + self.celldim['h'])
	vertex4 = (x, y + self.celldim['h'])
	self.draw.polygon((vertex1, vertex2, vertex3, vertex4),\
			  outline=outlinecolour, fill=colour)

class SquareArray(ComplexArray):
    def __init__(self, xy, arrayvalues, edgelen):
	ComplexArray.__init__(self, xy, arrayvalues, edgelen)
	self.celldim['w'] = edgelen
	self.celldim['h'] = edgelen
	image_width = self.celldim['w'] * self.dimensions[0] + self.padding * 2
	image_height = self.celldim['h'] * dimensions[1] + self.padding * 2
	self.im = Image.new("L", (image_width,image_height), self.bgcolour)
	self.draw = ImageDraw.Draw(self.im)

    def tile(self):
	for j in range(self.dimensions[1]):
	    x = self.padding
	    y = j * self.celldim['h'] + self.padding
	    for i in range (self.dimensions[0]):
		if (i != 0):
		    x = x + self.celldim['w']
		self._drawcell(x, y, self.values[(i,j)])

    def _drawcell(self, x, y, value):
	colour = value # (value, value, value)
	if drawline:
	    outlinecolour = linecolour
	else:
	    outlinecolour = colour
	vertex1 = (x, y)
	vertex2 = (x + self.celldim['w'], y)
	vertex3 = (x + self.celldim['w'], y + self.celldim['h'])
	vertex4 = (x, y + self.celldim['h'])
	self.draw.polygon((vertex1, vertex2, vertex3, vertex4),\
			  outline=outlinecolour, fill=colour)

class TrigArray(ComplexArray):
    def __init__(self, xy, arrayvalues, edgelen):
	ComplexArray.__init__(self, xy, arrayvalues, edgelen)
	self.celldim['w'] = self.edgelen
	self.celldim['h'] = math.floor(self.celldim['w'] * 0.5 * math.sqrt(3))
	image_width = self.celldim['w'] * (self.dimensions[0] * 0.5 +0.5) \
		      + self.padding * 2
	image_height = self.celldim['h'] * dimensions[1] + self.padding * 2
	self.im = Image.new("L", (image_width,image_height), self.bgcolour)
	self.draw = ImageDraw.Draw(self.im)

    def tile(self):
	x = self.padding
	y = self.padding
	for j in range(self.dimensions[1]):
	    x = self.padding
	    y = j * self.celldim['h'] + self.padding
	    if j % 2 == 0:
		x = x + self.edgelen * 0.5
	    for i in range (self.dimensions[0]):
		if (i + j) % 2 == 1:
		    mode = "EDGE_ON_TOP"
		else:
		    mode = "VERTEX_ON_TOP"
		    if (i != 0):
			x = x + self.celldim['w']
		self._drawcell(x, y, self.values[(i,j)], mode)

    def _drawcell(self, x, y, value, mode):
	colour = value #(value, value, value)
	if drawline:
	    outlinecolour = linecolour
	else:
	    outlinecolour = colour
	if mode == "EDGE_ON_TOP":
	    vertex1 = (x, y)
	    vertex2 = (x + self.edgelen, y)
	    vertex3 = (x + self.edgelen * 0.5, y + self.celldim['h'])

	elif mode == "VERTEX_ON_TOP":
	    vertex1 = (x, y)
	    vertex2 = (x + self.edgelen * 0.5, y + self.celldim['h'])
	    vertex3 = (x - self.edgelen * 0.5, y + self.celldim['h'])

	self.draw.polygon((vertex1, vertex2, vertex3), outline=outlinecolour, \
			    fill=colour)

class HexArray(ComplexArray):
    def __init__(self, xy, arrayvalues, edgelen):
	ComplexArray.__init__(self, xy, arrayvalues, edgelen)
	self.celldim['w'] = self.edgelen * math.sqrt(3)
	self.celldim['h'] = 2 * self.edgelen
	image_width = self.celldim['w'] * (self.dimensions[0] + 0.5) \
		      + self.padding * 2
	image_height = (self.celldim['h'] * 0.75 * self.dimensions[1]) \
		       + (self.padding * 2) + (self.celldim['h'] * 0.25)
	self.im = Image.new("L", (image_width, image_height), self.bgcolour)
	self.draw = ImageDraw.Draw(self.im)

    def tile(self):
	for j in range(self.dimensions[1]):
	    x = self.padding
	    y = self.padding + j * self.celldim['h'] * 0.75
	    if j % 2 == 1:
		x = x + self.celldim['w'] * 0.5
	    for i in range (self.dimensions[0]):
		self._drawcell(x, y, self.values[(i,j)])
		x = x + self.celldim['w']

    def _drawcell(self, x, y, value, mode=""):
	colour = value #(value, value, value)
	if drawline:
	    outlinecolour = linecolour
	else:
	    outlinecolour = colour
	vertex1 = (x + self.celldim['w'] * 0.5, y)
	vertex2 = (x + self.celldim['w'], y + self.celldim['h'] * 0.25)
	vertex3 = (vertex2[0], vertex2[1] + self.edgelen)
	vertex4 = (vertex1[0], vertex1[1] + self.celldim['h'])
	vertex5 = (vertex3[0]-self.celldim['w'], vertex3[1])
	vertex6 = (vertex5[0], vertex5[1]-self.edgelen)
	self.draw.polygon((vertex1, vertex2, vertex3, vertex4, vertex5, vertex6), \
			  outline=outlinecolour, fill=colour)

def nextframe(infile, maxlevel, dimensions):
    framevalues = {}
    line = infile.readline()
    linecount = 0
    linestoskip = 1000000
    while line:
	line = string.replace(line, '\"', '')
	wordlist = string.split(line);
	# These are quite fragile criteria for the beginning of a frame, but...
	if len(wordlist) == 4 and wordlist[0] != '/*':
	    # The third word is the number of lines to skip
	    linestoskip = string.atoi(wordlist[2])
	    linecount = 0
	    rowcount = 0
	if dimensions \
	   and linecount > linestoskip \
	   and linecount <=  linestoskip + dimensions[1]:
#	    print "inverse = %d" % (inverse)
	    for i in range(dimensions[0]):
		framevalues[(i, rowcount)] \
				= inverse * 255\
				+ (-1)**inverse\
				* (255 * string.atoi(line[i], maxlevel + 1) / maxlevel)
#		print "framevalues[(%d,%d)] = %d" % (i,rowcount,framevalues[(i, rowcount)])
	    rowcount = rowcount + 1
	if dimensions \
	   and linecount > linestoskip \
	   and linecount == linestoskip + dimensions[1]:
	    break
	if not dimensions and '}' in line:
	    break
	linecount = linecount + 1
	line = infile.readline()
    return framevalues

def print_usage():
    print "usage: arraydraw.py [-c] [-e edge_length] [-f framenum]" +\
		 "[-a [delay]] input_file"
    print "  -c: conserve images that have already been generated"
    print "  -i: invert greyscale"
    print "  -e edge_length: specify size of polygon by its edge " +\
			    "length (default=4)"
    print "  -f framenum: specify a frame or a range of frames to " +\
			    "generate images for"
    print "  -l line_colour: specify colour to be used for outlines"
    print "                  (no outlines if this option is unspecified)."

    print "  -a delay: create animation from the generated still images"
    print "              The delay between frames can be specified in " +\
			"units of 1/100 s"
    print "              (default = 1)."
    print "              You must have gifsicle installed for this option; " +\
			"if you don\'t"
    print "              have it, you can get it from: " +\
			"http://www.lcdf.org/gifsicle/."

if __name__ == "__main__":

    # First, parse arguments
    try:
	optlist, args = getopt.getopt(sys.argv[1:], 'ca:e:f:l:i')
    except getopt.error, msg:
	print msg
	print_usage()
	sys.exit(2)
    edge_length = None
    frames    = None
    delay       = None
    conserve    = 0
    makeanim    = 0
    drawline    = 0
    inverse     = 0
    for opt, arg in optlist:
	if opt == '-c': conserve = 1
	if opt == '-e': edge_length = arg
	if opt == '-f': frames = arg
	if opt == '-a': delay = arg
	if opt == '-l': drawline = arg
	if opt == '-i': inverse = 1
    if drawline:
	try:
	    linecolour = string.atoi(drawline)
	except ValueError:
	    print "Parameter [line_colour] must be an integer %s." % arg
	    print_usage()
	    sys.exit(2)
#    else:
#	linecolour = 0		
    if edge_length:
	try:
	    edge_length = string.atoi(edge_length)
	except ValueError:
	    print "Parameter [edge_length] must be an integer."
	    print_usage()
	    sys.exit(2)
    else:
	edge_length = 4
    if delay:
	makeanim = 1;
	try:
	    delay = string.atoi(delay)
	except ValueError:
	    print "Parameter [delay] must be an integer."
	    print_usage()
	    sys.exit(2)
    else:
	delay = 1
    if frames:
	frames = string.split(frames, ',')
	try:
	    for frametok in frames:
		framerange = string.split(frametok, '-')
		if len(framerange) == 1:
		    framenum = string.atoi(framerange[0])
		    frames = frames[1:] + [framenum]
		elif len(framerange) == 2:
		    startframe = string.atoi(framerange[0])
		    endframe = string. atoi(framerange[1])
		    frames = frames[1:] + range(startframe, endframe + 1)
	except ValueError:
	    print "Parameter [framenum] must be a comma-separated list of " +\
		  "integers or a range (e.g. '7,32-49,190,34,10-20')"
	    print_usage()
	    sys.exit(2)
	frames.sort()
    if len(args) != 1:
	print "Wrong number of arguments!!"
	print_usage()
	sys.exit(2)
    else:
	infilename = args[0]
	outfileprefix = string.join(string.split(infilename, '.')[:-1], '.')

    # Dictionary of ComplexArray constructors
    arraydict = {'Line': 'LineArray(dimensions, arrayvalues, edge_length)',
                 'Square': 'SquareArray(dimensions, arrayvalues, edge_length)',
		 'Trigonal': 'TrigArray(dimensions, arrayvalues, edge_length)',
		 'Hexagonal': 'HexArray(dimensions, arrayvalues, edge_length)',
		 'Triangle': 'TrigArray(dimensions, arrayvalues, edge_length)',
		 'Hexagon': 'HexArray(dimensions, arrayvalues, edge_length)'}

    # Initialise variables
    linecount = 0
    maxlevel = 0
    rowcount = 0
    arrayvalues = {}
    framelist = []
    filelist = []
    framecount = 0

    # open input file and get geometry and dimensions
    infile = open(infilename)
    line = infile.readline()
    while line:
	line = string.replace(line, '\"', '')
	wordlist = string.split(line);
	if "Geometry" in wordlist:
	    geometry = wordlist[2]
	# This is quite a fragile criteria for identifying this line, but...
	if len(wordlist) == 4 and wordlist[0] != '/*':
	    dimensions = (string.atoi(wordlist[0]), string.atoi(wordlist[1]))
	    maxlevel = string.atoi(wordlist[2]) - 1
	    break
	line = infile.readline()
    infile.close()

    # Re-open input file and process frames one by one
    infile = open(infilename)
    framecount = 0
    while 1:
	while frames and framecount != frames[0]:
	    nextframe(infile, maxlevel, None)
	    framecount = framecount + 1
	if frames:
	    frames = frames[1:]
	arrayvalues = nextframe(infile, maxlevel, dimensions)
	if arrayvalues == {}:
	    break
	outfilename = outfileprefix + "_%04d.gif" % (framecount)
	filelist.append(outfilename);
	if conserve == 0 or not os.path.exists(outfilename):
	    # Call constructor from dictionary (see definition of arraydict)
	    array = eval(arraydict[geometry])
	    array.tile()
	    del array.draw
	    print "Writing to output file:", outfilename, "..."
	    array.im.save(outfilename)
	    del array.im
	framecount = framecount + 1
	if frames == []:
	    break
    infile.close()

    # Generate animation file if requested (using external program gifsicle)
    if makeanim:
	animfilename = string.upper(outfileprefix) + "_ANIM.gif"
	anim_cmd = "gifsicle -O2 --no-loopcount -d %d " % (delay) \
		 + string.join(filelist, ' ') + " > " + animfilename
	print "Running gifsicle with delay = %d" % (delay)
	print anim_cmd
	os.system(anim_cmd)
