# -*- coding: utf-8 -*-
#
# Data Plane Software Design's Sphinx Configuration
#

# -- General Configuration --

project = 'Data Plane Software Design'
copyright = '2022, Ericsson AB'
author = 'Mattias Rönnblom'

# The short X.Y version
version = '0.0'
# The full version, including alpha/beta/rc tags
release = '0.0.2'


extensions = [
    'sphinx.ext.todo',
    'sphinx.ext.imgmath',
    'sphinx.ext.githubpages',
    'sphinxcontrib.plantuml',
    'sphinxcontrib.spelling',
]

templates_path = ['templates']

source_suffix = '.rst'

show_authors = True

master_doc = 'index'

spelling_warning=False

language = 'en_US'

highlight_language = 'C'

exclude_patterns = ['build']

pygments_style = None

# -- PlantUML Options --

plantuml_output_format = 'png'
plantuml_latex_output_format = 'pdf'

# -- HTML Options --

html_theme = 'alabaster'

# -- LaTeX Options --

latex_elements = {
    #'papersize': 'a4paper',
    #'pointsize': '10pt'
}

latex_documents = [
    (master_doc, 'DataPlaneSoftwareDesign.tex',
     'Data Plane Software Design',
     'Mattias Rönnblom', 'manual'),
]

# -- Texinfo Options --

texinfo_documents = [
    (master_doc, 'DataPlaneSoftwareDesign', 'Data Plane Software Design',
     author, 'DataPlaneSoftwareDesign',
     'A book on data plane software design.',
     'Miscellaneous'),
]

