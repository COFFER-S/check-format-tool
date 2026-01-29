# SPDX-FileCopyrightText: 2026 COFFER-S Systems (Shanghai) CO., LTD
#
# SPDX-License-Identifier: Apache-2.0

import setuptools

AUTHOR = 'COFFER'
MAINTAINER = 'COFFER'
EMAIL = ''

NAME = 'check-format'
SHORT_DESCRIPTION = 'The script for checking commit format'
LICENSE = 'Apache-2.0'
URL = 'https://github.com/COFFER-S/check-format-tool'
REQUIRES = [
    '',
]

setuptools.setup(
    
    name=NAME,
    description=SHORT_DESCRIPTION,
    long_description_content_type='text/markdown',
    license=LICENSE,
    version='0.0.1',
    author=AUTHOR,
    maintainer=MAINTAINER,
    author_email=EMAIL,
    url=URL,
    install_requires=REQUIRES,
    py_modules=['check_format'],
    scripts=['check_format.py'],
    entry_points={'console_scripts': ['check-format=check_format:main']},
)
