//
// Created by frongere on 03/04/18.
//

#ifndef HERMES_HERMES_H
#define HERMES_HERMES_H

#include "hermes/message.h"
#include "hermes/serializer/print_serializer.h"
#include "hermes/serializer/csv_serializer.h"

/**
 * Concerning the log levels
 *
 * Log levels are integers between 0 and 4 (5 log level into hermes)

 * Log levels can be optionally set on each field of a message while adding field to a message.
 * By default, the log level of a field is set to 0.
 *
 * A log level can be set at the level of a message. By default the message log level is set to max 4.
 * This means that every field is logged.
 *
 * Setting the message's log level eg to 2 means that only messages having a log level inferior or equal to 2 will be
 * logged
 *
 */

#endif //HERMES_HERMES_H
