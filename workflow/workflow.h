/*
 *******************************************************************************
 * Copyright © 2016 Sviatoslav Semchyshyn
 *******************************************************************************
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *******************************************************************************
 */
#ifndef _WORKFLOW_H_
#define _WORKFLOW_H_

#include <stdint.h>

void cdc_receive_callback(uint8_t* buf, uint32_t len);
int workflow();

#endif /* _WORKFLOW_H_ */
