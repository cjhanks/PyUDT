from distutils.core import setup, Extension 

udt4 = Extension(
        'udt4',
        sources         = ['py-udt4.cc'],
        include_dirs    = ['/usr/local/include'],
        libraries       = ['udt'],
        library_dirs    = ['/usr/local/lib'],
        extra_link_args = ['-g -O3 -Wall -Wextra']
        )

setup(
        name            = 'py-udt',
        version         = '0.1',
        description     = 'Python bindings for UDT4',
        author          = 'Christopher J. Hanks',
        author_email    = 'develop@cjhanks.name',
        url             = 'http://cjhanks.name/projects/py-udt.php',
        #py_modules      = ['udt4'],
        ext_modules     = [udt4] 
    ) 
