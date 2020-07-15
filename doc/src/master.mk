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

# common make meta rules and vars for all subdirectories

XPATH=XFM_PATH=.:/usr/local/share/xfm TFM_PATH=.:/usr/local/share/xfm

# use care: the output type is used to source the macros based on the type
# of output being rendered.
# Two pass builds allow for table of contents gen/insert, forward var
# references etc.
#
%.ps: %.xfm
	$(XPATH) OUTPUT_TYPE=generic_ps XFM_PASS=1 pfm $< /dev/null
	$(XPATH) OUTPUT_TYPE=generic_ps XFM_PASS=2 pfm $< $@

%.md: %.xfm
	$(XPATH) OUTPUT_TYPE=markdown XFM_PASS=1 tfm $< /dev/null
	$(XPATH) OUTPUT_TYPE=markdown XFM_PASS=2 tfm $< | sed 's/^ //' >$@

%.rst: %.xfm
	$(XPATH) OUTPUT_TYPE=rst XFM_PASS=1 tfm $< /dev/null
	$(XPATH) OUTPUT_TYPE=rst XFM_PASS=2 tfm $< | sed 's/^ //; s/ *$$//' >$@

%.txt: %.xfm
	$(XPATH) OUTPUT_TYPE=txt XFM_PASS=1 tfm $< /dev/null
	$(XPATH) OUTPUT_TYPE=txt XFM_PASS=2 tfm $< $@
