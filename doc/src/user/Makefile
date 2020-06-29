#==================================================================================
#   Copyright (c) 2020 Nokia
#   Copyright (c) 2020 AT&T Intellectual Property.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#==================================================================================

# this uses {X}fm which can be cloned from: https://gitlab.com/rouxware/xfm

docs = user_guide
src = user_guide.xfm
imbed_src = cpp_frame.im example1.im example2.im example3.im jhash.im
desired_out = rst ps

# use care: the output type is used to source the macros based on the type
# of output being rendered.
# Two pass builds allow for table of contents gen/insert, forward var
# references etc.
#
%.ps: %.xfm
	OUTPUT_TYPE=generic_ps XFM_PASS=1 pfm $< /dev/null
	OUTPUT_TYPE=generic_ps XFM_PASS=2 pfm $< $@

%.md: %.xfm
	OUTPUT_TYPE=markdown XFM_PASS=1 tfm $< /dev/null
	OUTPUT_TYPE=markdown XFM_PASS=2 tfm $< | sed 's/^ //' >$@

%.rst: %.xfm
	OUTPUT_TYPE=rst XFM_PASS=1 tfm $< /dev/null
	GEN_TITLE=1 OUTPUT_TYPE=rst XFM_PASS=2 tfm $< | sed 's/^ //' >$@

%.txt: %.xfm
	OUTPUT_TYPE=txt XFM_PASS=1 tfm $< /dev/null
	OUTPUT_TYPE=txt XFM_PASS=2 tfm $< $@

# -----------------------------------------------------------------------------------
all:    $(desired_out:%=user_guide.%)

user_guide.ps: user_guide.xfm $(imbed_src)

# we force the .rst docs to always be out of date so that we don't have to
# jump hoops to ensure that they build for make publish. (See hack rule
# at the end of the file.)
#
$(docs:%=%.rst): always

publish: user_guide.rst
	cp user_guide.rst ../../../docs/user-guide.rst

# intermeidate junk that might straggle
clean:
	rm -fr *.bcnfile *.ca *.ecnfile *.sp

# Destroy anything that can be built
nuke: clean
	rm -fr *.md *.ps *.pdf *.txt *.rst *.toc

# make hack to force a rule to always be out of date
always:

