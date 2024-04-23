/*
 * Copyright (C) 2016 John Chou <luckyjoou@gmail.com>
 * Copyright (C) 2016 David Phillip Oster.
 *
 * Licensed under the MIT License,
 *                http://opensource.org/licenses/mit-license.php
 *
 * ezxml wrapper for DOM manipulation.
 */

#ifndef SVGTINY_XML2DOM_H
#define SVGTINY_XML2DOM_H

#include <stdint.h>
#include <stdlib.h>
#include "ezxml.h"

typedef struct dom_node {
    ezxml_t node;
    int ref;
} dom_node;

typedef dom_node dom_document;
typedef dom_node dom_element;

typedef struct dom_xml_parser {
    // struct _xmlDoc *parser;
    dom_document *doc;
} dom_xml_parser;

typedef enum {
    DOM_NO_ERR = 0,
    DOM_FILE_ERR = 1,
    DOM_MEM_ERR = 2,
} dom_exception;

typedef enum { lwc_error_ok = 0, lwc_error_bad } lwc_error;

typedef enum {
    DOM_ELEMENT_NODE,
    DOM_ATTRIBUTE_NODE,
    DOM_TEXT_NODE,
    DOM_COMMENT_NODE,
    DOM_DOCUMENT_NODE,
    DOM_NODELIST_NODE,
    DOM_OTHER_NODE,
} dom_node_type;

typedef enum {
    DOM_XML_OK = 0,
    DOM_XML_MALFORMED = 1,
} dom_xml_error;

typedef struct dom_nodelist {
    dom_node **nodes;  // alocated on the heap.
    int ref;
    int count;
} dom_nodelist;

typedef struct dom_string {
    char *s;  // alocated on the heap.
    int ref;
} dom_string;

typedef dom_string lwc_string;

typedef void (*MesgFuncPtr)(uint32_t severity, void *ctx, const char *msg, ...);

dom_xml_parser *dom_xml_parser_create(void *dontCare1,
                                      void *dontCare2,
                                      MesgFuncPtr mesgFunc,
                                      void *dontCare3,
                                      dom_document **outDocument);
dom_xml_error dom_xml_parser_parse_chunk(dom_xml_parser *parser,
                                         const uint8_t *data,
                                         size_t len);
dom_xml_error dom_xml_parser_completed(dom_xml_parser *parser);
dom_xml_error dom_xml_parser_destroy(dom_xml_parser *parser);
dom_exception dom_document_get_document_element(dom_document *document,
                                                dom_element **outNode);
dom_exception dom_document_get_element_by_id(dom_node *node,
                                             dom_string *string,
                                             dom_element **outNode);
dom_exception dom_element_get_attribute(dom_node *node,
                                        dom_string *string,
                                        dom_string **outAttribute);
dom_exception dom_element_get_elements_by_tag_name(dom_element *element,
                                                   dom_string *string,
                                                   dom_nodelist **outNodeList);
dom_exception dom_node_get_node_name(dom_node *node, dom_string **outString);
void dom_node_unref(dom_node *node);
dom_exception dom_nodelist_get_length(dom_nodelist *nodeList, uint32_t *outLen);
dom_exception dom_nodelist_item(dom_nodelist *nodeList,
                                uint32_t index,
                                dom_node **outItemp);
void dom_nodelist_unref(dom_nodelist *nodeList);
dom_exception dom_node_get_first_child(dom_element *element,
                                       dom_element **outChild);
dom_exception dom_node_get_node_type(dom_node *node, dom_node_type *outType);
int dom_string_caseless_isequal(dom_string *a, dom_string *b);
dom_exception dom_node_get_next_sibling(dom_element *element,
                                        dom_element **outChild);
dom_exception dom_text_get_whole_text(dom_element *element,
                                      dom_string **outString);
lwc_error lwc_intern_string(const char *data,
                            size_t len,
                            lwc_string **outString);
int dom_string_caseless_lwc_isequal(dom_string *str, lwc_string *lwcString);
void lwc_string_unref(lwc_string *lwcString);
uint32_t dom_string_byte_length(dom_string *str);
dom_exception dom_string_create_interned(const uint8_t *data,
                                         size_t len,
                                         dom_string **outString);
char *dom_string_data(dom_string *str);
int dom_string_isequal(dom_string *a, dom_string *b);
dom_string *dom_string_ref(dom_string *str);
void dom_string_unref(dom_string *str);

char *strduplower2(const uint8_t *data, size_t len);
char *strduplower(const char *s);

#endif  // SVGTINY_XML2DOM_H
