/* ezxml wrapper for DOM manipulation.
 *
 * Copyright (C) 2016 John Chou.
 * Copyright (C) 2016 David Phillip Oster.
 * Released under the MIT License, https://opensource.org/license/mit
 */

#include "dom.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

enum { MAGIC_DOCUMENT_NODE = 10000 };

static void lower(char *s)
{
    for (; *s; ++s) {
        if (isupper(*s))
            *s = tolower(*s);
    }
}

static char *my_strdup(const uint8_t *data, size_t len)
{
    char *result = malloc(len + 1);
    memcpy(result, data, len);
    result[len] = '\0';
    return result;
}

dom_xml_parser *dom_xml_parser_create(void *dontCare1,
                                      void *dontCare2,
                                      MesgFuncPtr mesgFunc,
                                      void *dontCare3,
                                      dom_document **outDocument)
{
    dom_document *docResult = calloc(sizeof(dom_document), 1);
    *outDocument = docResult;
    dom_xml_parser *result = calloc(sizeof(dom_xml_parser), 1);
    result->doc = docResult;
    return result;
}

dom_xml_error dom_xml_parser_parse_chunk(dom_xml_parser *parser,
                                         const uint8_t *data,
                                         size_t len)
{
    assert(parser);
    ezxml_t doc = ezxml_parse_str((char *) data, len);
    if (doc) {
        doc->type = XML_DOCUMENT_NODE;
        parser->doc->node = doc;
        parser->doc->ref = MAGIC_DOCUMENT_NODE;
        return DOM_XML_OK;
    }
    return DOM_XML_MALFORMED;
}

dom_xml_error dom_xml_parser_completed(dom_xml_parser *parser)
{
    return DOM_XML_OK;
}

dom_xml_error dom_xml_parser_destroy(dom_xml_parser *parser)
{
    // we intentionally don't free the document here.
    free(parser);
    return DOM_XML_OK;
}

dom_exception dom_document_get_document_element(dom_document *document,
                                                dom_element **outNode)
{
    dom_element *element = calloc(sizeof(dom_element), 1);
    element->node = document->node;
    element->ref = 1;
    *outNode = element;
    return DOM_NO_ERR;
}

dom_exception dom_document_get_element_by_id(dom_node *node,
                                             dom_string *string,
                                             dom_element **outNode)
{
    const char *id = ezxml_attr(node->node, "id");
    if (id && 0 == strcasecmp(id, string->s)) {
        node->ref++;
        *outNode = node;
        return DOM_NO_ERR;
    }
    *outNode = NULL;
    /* TODO: Find id recursively. */
    return DOM_NO_ERR;
}

dom_exception dom_element_get_attribute(dom_node *node,
                                        dom_string *string,
                                        dom_string **outAttribute)
{
    const char *attr = ezxml_attr(node->node, string->s);
    if (attr)
        return dom_string_create_interned((const uint8_t *) attr, strlen(attr),
                                          outAttribute);
    if (strchr(string->s, ':'))
        fprintf(stderr, "TODO:dom_element_get_attribute - namespace %s\n",
                string->s);
    *outAttribute = NULL;
    return DOM_NO_ERR;
}

dom_exception dom_element_get_elements_by_tag_name(dom_element *element,
                                                   dom_string *string,
                                                   dom_nodelist **outNodeList)
{
    dom_nodelist *result = NULL;
    dom_element **nodeList = NULL;
    int nodeCount = 0;
    for (ezxml_t candidate = ezxml_child(element->node, string->s); candidate;
         candidate = candidate->next) {
        if (!nodeList) {
            nodeList = malloc(nodeCount * sizeof(dom_element *));
        } else {
            dom_element **t =
                realloc(nodeList, (1 + nodeCount) * sizeof(dom_element *));
            if (t) {
                nodeList = t;
            } else {
                for (int i = 0; i < nodeCount; ++i)
                    dom_node_unref(nodeList[i]);
                free(nodeList);
                *outNodeList = result;
                return DOM_MEM_ERR;
            }
        }
        dom_element *elem = calloc(sizeof(dom_element), 1);
        elem->node = candidate;
        elem->ref = 1;
        nodeList[nodeCount++] = elem;
    }
    if (nodeCount) {
        result = calloc(sizeof(dom_nodelist), 1);
        result->nodes = nodeList;
        result->count = nodeCount;
        result->ref = 1;
    }
    *outNodeList = result;
    return DOM_NO_ERR;
}

dom_exception dom_node_get_node_name(dom_node *node, dom_string **outString)
{
    ezxml_t n = node->node;
    dom_exception errCode = DOM_NO_ERR;
    errCode = dom_string_create_interned((const uint8_t *) n->name,
                                         strlen(n->name), outString);
    return errCode;
}

void dom_node_unref(dom_node *node)
{
    assert(node);
    if (MAGIC_DOCUMENT_NODE == node->ref) {
        ezxml_free(node->node);
        free(node);
    } else {
        node->ref--;
        if (0 == node->ref)
            free(node);
    }
}

dom_exception dom_nodelist_get_length(dom_nodelist *nodeList, uint32_t *outLen)
{
    *outLen = nodeList->count;
    return DOM_NO_ERR;
}

dom_exception dom_nodelist_item(dom_nodelist *nodeList,
                                uint32_t index,
                                dom_node **outItemp)
{
    dom_node *node = nodeList->nodes[index];
    node->ref++;
    *outItemp = node;
    return DOM_NO_ERR;
}

void dom_nodelist_unref(dom_nodelist *nodeList)
{
    assert(nodeList);
    nodeList->ref--;
    if (0 == nodeList->ref) {
        int count = nodeList->count;
        for (int i = 0; i < count; ++i)
            dom_node_unref(nodeList->nodes[i]);
        if (count > 0)
            free(nodeList->nodes);
        free(nodeList);
    }
}

dom_exception dom_node_get_first_child(dom_element *element,
                                       dom_element **outChild)
{
    dom_element *newElement = NULL;
    ezxml_t child = element->node->child;
    if (child) {
        newElement = calloc(sizeof(dom_element), 1);
        newElement->node = child;
        newElement->ref = 1;
    }
    *outChild = newElement;
    return DOM_NO_ERR;
}

dom_exception dom_node_get_node_type(dom_node *node, dom_node_type *outType)
{
    switch (node->node->type) {
    case XML_ELEMENT_NODE:
        *outType = DOM_ELEMENT_NODE;
        break;
    case XML_ATTRIBUTE_NODE:
        *outType = DOM_ATTRIBUTE_NODE;
        break;
    case XML_CDATA_SECTION_NODE:
    case XML_TEXT_NODE:
        *outType = DOM_TEXT_NODE;
        break;
    case XML_COMMENT_NODE:
        *outType = DOM_COMMENT_NODE;
        break;
    case XML_DOCB_DOCUMENT_NODE:
    case XML_DOCUMENT_NODE:
    case XML_DOCUMENT_TYPE_NODE:
    case XML_DOCUMENT_FRAG_NODE:
    case XML_HTML_DOCUMENT_NODE:
        *outType = DOM_DOCUMENT_NODE;
        break;
    default:
        *outType = DOM_OTHER_NODE;
        break;
    }
    return DOM_NO_ERR;
}

int dom_string_caseless_isequal(dom_string *as, dom_string *bs)
{
    return !strcasecmp(as->s, bs->s);
}

dom_exception dom_node_get_next_sibling(dom_element *element,
                                        dom_element **outChild)
{
    dom_element *nextElement = NULL;
    ezxml_t child = element->node->ordered;

    if (child) {
        nextElement = (dom_element *) calloc(sizeof(dom_element), 1);
        nextElement->node = child;
        nextElement->ref = 1;
    }
    *outChild = nextElement;
    return DOM_NO_ERR;
}

dom_exception dom_text_get_whole_text(dom_element *element,
                                      dom_string **outString)
{
    return DOM_NO_ERR;
}

lwc_error lwc_intern_string(const char *data,
                            size_t len,
                            lwc_string **outString)
{
    return (lwc_error) dom_string_create_interned((const uint8_t *) data, len,
                                                  outString);
}

int dom_string_caseless_lwc_isequal(dom_string *str, lwc_string *lwcString)
{
    return dom_string_caseless_isequal(str, lwcString);
}

void lwc_string_unref(lwc_string *lwcString)
{
    dom_string_unref(lwcString);
}

uint32_t dom_string_byte_length(dom_string *str)
{
    return (uint32_t) strlen(str->s);
}

dom_exception dom_string_create_interned(const uint8_t *data,
                                         size_t len,
                                         dom_string **outString)
{
    dom_string *newStr = calloc(sizeof(dom_string), 1);
    newStr->s = my_strdup(data, len);
    newStr->ref = 1;
    *outString = newStr;
    return DOM_NO_ERR;
}

char *dom_string_data(dom_string *str)
{
    return str->s;
}

int dom_string_isequal(dom_string *a, dom_string *b)
{
    return !strcmp(a->s, b->s);
}

dom_string *dom_string_ref(dom_string *str)
{
    str->ref++;
    return str;
}

void dom_string_unref(dom_string *str)
{
    str->ref--;
    if (0 == str->ref) {
        free(str->s);
        free(str);
    }
}

char *strduplower2(const uint8_t *data, size_t len)
{
    char *lowerStr = my_strdup(data, len);
    lower(lowerStr);
    return lowerStr;
}

char *strduplower(const char *s)
{
    return strduplower2((const uint8_t *) s, strlen(s));
}
