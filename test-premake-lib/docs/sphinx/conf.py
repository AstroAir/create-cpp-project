# Configuration file for the Sphinx documentation builder.

project = 'test-premake-lib'
copyright = '2023, Author Name'
author = 'Author Name'
version = '0.1'
release = '0.1.0'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.coverage',
    'sphinx.ext.napoleon',
    'breathe',
    'sphinx_rtd_theme',
]

templates_path = ['_templates']
source_suffix = '.rst'
master_doc = 'index'
language = None
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']
pygments_style = 'sphinx'

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

# Breathe configuration
breathe_projects = { "test-premake-lib": "../doxygen/xml" }
breathe_default_project = "test-premake-lib"
breathe_default_members = ('members', 'undoc-members')
