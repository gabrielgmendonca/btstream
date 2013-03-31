/*
 * Copyright (C) 2011-2013 Gabriel Mendonça
 *
 * This file is part of BTStream.
 * BTStream is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BTStream is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with BTStream.  If not, see <http://www.gnu.org/licenses/>.
 *
 * PropertyInstaller.h
 *
 *  Created on: 03/02/2012
 *      Author: gabriel
 */

#ifndef PROPERTYINSTALLER_H_
#define PROPERTYINSTALLER_H_

#include <glib-object.h>

/**
 * Helper class that installs properties for a GObject class.
 * Properties default to read-only.
 */
class PropertyInstaller {
public:
	PropertyInstaller(GObjectClass* gobject_class);

	void install_bool(unsigned int property, const char* name,
			const char* nickname, const char* description, bool default_value =
					false, bool writable = false);

	void install_int(unsigned int property, const char* name,
			const char* nickname, const char* description, int min = 0,
			int max = 1, int default_value = 0, bool writable = false);

	void install_float(unsigned int property, const char* name,
			const char* nickname, const char* description, float min = 0.0f,
			float max = 1.0f, float default_value = 0.0f, bool writable = false);

	void install_string(unsigned int property, const char* name,
			const char* nickname, const char* description,
			const char* default_value = "", bool writable = false);

	void install_property(unsigned int property, GParamSpec* param_spec);

private:
	GObjectClass* m_gobject_class;

	GParamFlags create_flags(bool writable);
};

#endif /* PROPERTYINSTALLER_H_ */
