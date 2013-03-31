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
 * PropertyInstaller.cpp
 *
 *  Created on: 03/02/2012
 *      Author: gabriel
 */

#include "propertyinstaller.h"

PropertyInstaller::PropertyInstaller(GObjectClass* gobject_class) :
		m_gobject_class(gobject_class) {
}

void PropertyInstaller::install_property(unsigned int property,
		GParamSpec* param_spec) {
	g_object_class_install_property(m_gobject_class, property, param_spec);
}

void PropertyInstaller::install_bool(unsigned int property,
		const char* name, const char* nickname, const char* description,
		bool default_value, bool writable) {

	GParamSpec* param_spec;
	param_spec = g_param_spec_boolean(name, nickname, description,
			default_value, create_flags(writable));

	install_property(property, param_spec);
}

void PropertyInstaller::install_int(unsigned int property,
		const char* name, const char* nickname, const char* description,
		int min, int max, int default_value, bool writable) {

	GParamSpec* param_spec;
	param_spec = g_param_spec_int(name, nickname, description, min, max,
			default_value, create_flags(writable));

	install_property(property, param_spec);
}

void PropertyInstaller::install_float(unsigned int property, const char* name,
		const char* nickname, const char* description, float min, float max,
		float default_value, bool writable) {

	GParamSpec* param_spec;
	param_spec = g_param_spec_float(name, nickname, description, min, max,
			default_value, create_flags(writable));

	install_property(property, param_spec);
}

void PropertyInstaller::install_string(unsigned int property,
		const char* name, const char* nickname, const char* description,
		const char* default_value, bool writable) {

	GParamSpec* param_spec;
	param_spec = g_param_spec_string(name, nickname, description, default_value,
			create_flags(writable));

	install_property(property, param_spec);
}

GParamFlags PropertyInstaller::create_flags(bool writable) {
	if (writable) {
		return static_cast<GParamFlags>(G_PARAM_READWRITE);
	} else {
		return static_cast<GParamFlags>(G_PARAM_READABLE);
	}
}
