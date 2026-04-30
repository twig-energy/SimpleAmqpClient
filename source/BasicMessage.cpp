/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

// Put these first to avoid warnings about INT#_C macro redefinition
#include "SimpleAmqpClient/BasicMessage.h"

#include <cstring>
#include <optional>
#include <string>
#include <utility>

#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/framing.h>

namespace AmqpClient {

struct BasicMessage::Impl {
  std::string body;
  std::optional<std::string> content_type;
  std::optional<std::string> content_encoding;
  std::optional<delivery_mode_t> delivery_mode;
  std::optional<uint8_t> priority;
  std::optional<std::string> correlation_id;
  std::optional<std::string> reply_to;
  std::optional<std::string> expiration;
  std::optional<std::string> message_id;
  std::optional<uint64_t> timestamp;
  std::optional<std::string> type;
  std::optional<std::string> user_id;
  std::optional<std::string> app_id;
  std::optional<std::string> cluster_id;
  std::optional<Table> header_table;
};

BasicMessage::BasicMessage() : m_impl(new Impl) {}

BasicMessage::BasicMessage(std::string body) : m_impl(new Impl) {
  Body(std::move(body));
}

BasicMessage::~BasicMessage() {}

const std::string& BasicMessage::Body() const { return m_impl->body; }

std::string& BasicMessage::Body() { return m_impl->body; }

void BasicMessage::Body(std::string body) { m_impl->body = std::move(body); }

const std::string& BasicMessage::ContentType() const {
  if (ContentTypeIsSet()) {
    return m_impl->content_type.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ContentType(std::string content_type) {
  m_impl->content_type = std::move(content_type);
}

bool BasicMessage::ContentTypeIsSet() const {
  return m_impl->content_type.has_value();
}

void BasicMessage::ContentTypeClear() { m_impl->content_type.reset(); }

const std::string& BasicMessage::ContentEncoding() const {
  if (ContentEncodingIsSet()) {
    return m_impl->content_encoding.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ContentEncoding(std::string content_encoding) {
  m_impl->content_encoding = std::move(content_encoding);
}

bool BasicMessage::ContentEncodingIsSet() const {
  return m_impl->content_encoding.has_value();
}

void BasicMessage::ContentEncodingClear() { m_impl->content_encoding.reset(); }

BasicMessage::delivery_mode_t BasicMessage::DeliveryMode() const {
  return m_impl->delivery_mode.value_or(dm_notset);
}

void BasicMessage::DeliveryMode(delivery_mode_t delivery_mode) {
  m_impl->delivery_mode = delivery_mode;
}

bool BasicMessage::DeliveryModeIsSet() const {
  return m_impl->delivery_mode.has_value();
}

void BasicMessage::DeliveryModeClear() { m_impl->delivery_mode.reset(); }

uint8_t BasicMessage::Priority() const {
  return m_impl->priority.value_or(0);
}

void BasicMessage::Priority(uint8_t priority) {
  m_impl->priority = priority;
}

bool BasicMessage::PriorityIsSet() const {
  return m_impl->priority.has_value();
}

void BasicMessage::PriorityClear() { m_impl->priority.reset(); }

const std::string& BasicMessage::CorrelationId() const {
  if (CorrelationIdIsSet()) {
    return m_impl->correlation_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::CorrelationId(std::string correlation_id) {
  m_impl->correlation_id = std::move(correlation_id);
}

bool BasicMessage::CorrelationIdIsSet() const {
  return m_impl->correlation_id.has_value();
}

void BasicMessage::CorrelationIdClear() { m_impl->correlation_id.reset(); }

const std::string& BasicMessage::ReplyTo() const {
  if (ReplyToIsSet()) {
    return m_impl->reply_to.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ReplyTo(std::string reply_to) {
  m_impl->reply_to = std::move(reply_to);
}

bool BasicMessage::ReplyToIsSet() const {
  return m_impl->reply_to.has_value();
}

void BasicMessage::ReplyToClear() { m_impl->reply_to.reset(); }

const std::string& BasicMessage::Expiration() const {
  if (ExpirationIsSet()) {
    return m_impl->expiration.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::Expiration(std::string expiration) {
  m_impl->expiration = std::move(expiration);
}

bool BasicMessage::ExpirationIsSet() const {
  return m_impl->expiration.has_value();
}

void BasicMessage::ExpirationClear() { m_impl->expiration.reset(); }

const std::string& BasicMessage::MessageId() const {
  if (MessageIdIsSet()) {
    return m_impl->message_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::MessageId(std::string message_id) {
  m_impl->message_id = std::move(message_id);
}

bool BasicMessage::MessageIdIsSet() const {
  return m_impl->message_id.has_value();
}

void BasicMessage::MessageIdClear() { m_impl->message_id.reset(); }

uint64_t BasicMessage::Timestamp() const {
  return m_impl->timestamp.value_or(0);
}
void BasicMessage::Timestamp(uint64_t timestamp) {
  m_impl->timestamp = timestamp;
}

bool BasicMessage::TimestampIsSet() const {
  return m_impl->timestamp.has_value();
}

void BasicMessage::TimestampClear() { m_impl->timestamp.reset(); }

const std::string& BasicMessage::Type() const {
  if (TypeIsSet()) {
    return m_impl->type.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::Type(std::string type) { m_impl->type = std::move(type); }

bool BasicMessage::TypeIsSet() const { return m_impl->type.has_value(); }

void BasicMessage::TypeClear() { m_impl->type.reset(); }

const std::string& BasicMessage::UserId() const {
  if (UserIdIsSet()) {
    return m_impl->user_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::UserId(std::string user_id) {
  m_impl->user_id = std::move(user_id);
}

bool BasicMessage::UserIdIsSet() const {
  return m_impl->user_id.has_value();
}

void BasicMessage::UserIdClear() { m_impl->user_id.reset(); }

const std::string& BasicMessage::AppId() const {
  if (AppIdIsSet()) {
    return m_impl->app_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::AppId(std::string app_id) { m_impl->app_id = std::move(app_id); }

bool BasicMessage::AppIdIsSet() const {
  return m_impl->app_id.has_value();
}

void BasicMessage::AppIdClear() { m_impl->app_id.reset(); }

const std::string& BasicMessage::ClusterId() const {
  if (ClusterIdIsSet()) {
    return m_impl->cluster_id.value();
  }
  static const std::string empty;
  return empty;
}

void BasicMessage::ClusterId(std::string cluster_id) {
  m_impl->cluster_id = std::move(cluster_id);
}

bool BasicMessage::ClusterIdIsSet() const {
  return m_impl->cluster_id.has_value();
}

void BasicMessage::ClusterIdClear() { m_impl->cluster_id.reset(); }

Table& BasicMessage::HeaderTable() {
  if (!HeaderTableIsSet()) {
    m_impl->header_table = Table();
  }
  return m_impl->header_table.value();
}

const Table& BasicMessage::HeaderTable() const {
  if (HeaderTableIsSet()) {
    return m_impl->header_table.value();
  }
  static const Table empty;
  return empty;
}

void BasicMessage::HeaderTable(const Table& header_table) {
  m_impl->header_table = header_table;
}

bool BasicMessage::HeaderTableIsSet() const {
  return m_impl->header_table.has_value();
}

void BasicMessage::HeaderTableClear() { m_impl->header_table.reset(); }

}  // namespace AmqpClient
