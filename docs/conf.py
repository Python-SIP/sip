# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

from datetime import date


# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'sip'
copyright = '{0} Phil Thompson <phil@riverbankcomputing.com>'.format(
        date.today().year)
author = 'Phil Thompson'
version = 'v6.12.0'


# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['myst_parser', 'sphinx.ext.intersphinx']
intersphinx_mapping = {'python': ('https://docs.python.org/3', None)}

#templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
#html_static_path = ['_static']


# MyST configuration
myst_enable_extensions = {'deflist'}


# -- Project-specific extensions -----------------------------------------------

def setup(app):
    """ Define roles specific to SIP. """

    app.add_object_type('argument-annotation', 'aanno',
            indextemplate='single: %s (argument annotation)')

    app.add_object_type('class-annotation', 'canno',
            indextemplate='single: %s (class annotation)')

    app.add_object_type('enum-annotation', 'eanno',
            indextemplate='single: %s (enum annotation)')

    app.add_object_type('exception-annotation', 'xanno',
            indextemplate='single: %s (exception annotation)')

    app.add_object_type('function-annotation', 'fanno',
            indextemplate='single: %s (function annotation)')

    app.add_object_type('mapped-type-annotation', 'manno',
            indextemplate='single: %s (mapped type annotation)')

    app.add_object_type('typedef-annotation', 'tanno',
            indextemplate='single: %s (typedef annotation)')

    app.add_object_type('variable-annotation', 'vanno',
            indextemplate='single: %s (variable annotation)')

    app.add_object_type('sip-type', 'stype',
            indextemplate='single: %s (SIP type)')

    app.add_crossref_type('directive', 'directive',
            indextemplate='single: %s (directive)')
