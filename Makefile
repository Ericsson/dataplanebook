SPHINXOPTS    =
SPHINXBUILD   = sphinx-build
SOURCEDIR     = src
BUILDDIR      = build
LINKCHECKDIR  = $(BUILDDIR)/linkcheck

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: check
check: checkspelling checklinks

.PHONY: checkspelling
checkspelling:
	@$(SPHINXBUILD) -M $@ $(SPHINXOPTS) "$(SOURCEDIR)" "$(BUILDDIR)" \
		-b spelling

.PHONY: checklinks
checklinks:
	$(SPHINXBUILD) $(SPHINXOPTS) "$(SOURCEDIR)" "$(LINKCHECKDIR)" \
		-b linkcheck
