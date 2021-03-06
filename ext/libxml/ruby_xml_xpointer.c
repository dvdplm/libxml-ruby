/* $Id$ */

/* Please see the LICENSE file for copyright and distribution information */

#include "ruby_libxml.h"
#include "ruby_xml_xpointer.h"

VALUE cXMLXPointer;
VALUE eXMLXPointerInvalidExpression;

VALUE
ruby_xml_xpointer_point(VALUE class, VALUE rnode, VALUE xptr_str) {
#ifdef LIBXML_XPTR_ENABLED
  xmlNodePtr xnode;
  xmlXPathContextPtr xctxt;
  xmlXPathObjectPtr xpop;
  
  VALUE context;
  VALUE result;
  VALUE argv[1];

  Check_Type(xptr_str, T_STRING);
  if (rb_obj_is_kind_of(rnode, cXMLNode) == Qfalse)
    rb_raise(rb_eTypeError, "require an XML::Node object");

  Data_Get_Struct(rnode, xmlNode, xnode);

  argv[0] = rb_funcall(rnode, rb_intern("doc"), 0);
  context = rb_class_new_instance(1, argv, cXMLXPathContext);
  Data_Get_Struct(context, xmlXPathContext, xctxt);

  xpop = xmlXPtrEval((xmlChar*)StringValuePtr(xptr_str), xctxt);
  if (!xpop)
    rb_raise(eXMLXPointerInvalidExpression, "Invalid xpointer expression");

  result = ruby_xml_xpath_object_wrap(xpop);
  rb_iv_set(result, "@context", context);
  
  return(result);
#else
  rb_warn("libxml was compiled without XPointer support");
  return(Qfalse);
#endif
}


VALUE
ruby_xml_xpointer_point2(VALUE node, VALUE xptr_str) {
  return(ruby_xml_xpointer_point(cXMLXPointer, node, xptr_str));
}


/*
 * call-seq:
 *    XML::XPointer.range(start_node, end_node) -> xpath
 * 
 * Create an xpath representing the range between the supplied
 * start and end node.
 */
VALUE
ruby_xml_xpointer_range(VALUE class, VALUE rstart, VALUE rend) {
#ifdef LIBXML_XPTR_ENABLED
  xmlNodePtr start, end;
  VALUE rxxp;
  xmlXPathObjectPtr xpath;

  if (rb_obj_is_kind_of(rstart, cXMLNode) == Qfalse)
    rb_raise(rb_eTypeError, "require an XML::Node object as a starting point");
  if (rb_obj_is_kind_of(rend, cXMLNode) == Qfalse)
    rb_raise(rb_eTypeError, "require an XML::Node object as an ending point");

  Data_Get_Struct(rstart, xmlNode, start);
  if (start == NULL)
    return(Qnil);

  Data_Get_Struct(rend, xmlNode, end);
  if (end == NULL)
    return(Qnil);

  xpath = xmlXPtrNewRangeNodes(start, end);
  if (xpath == NULL)
    rb_fatal("You shouldn't be able to have this happen");

  rxxp = ruby_xml_xpath_object_wrap(xpath);
  return(rxxp);
#else
  rb_warn("libxml was compiled without XPointer support");
  return(Qfalse);
#endif
}

// Rdoc needs to know 
#ifdef RDOC_NEVER_DEFINED
  mLibXML = rb_define_module("LibXML");
  mXML = rb_define_module_under(mLibXML, "XML");
#endif

void
ruby_init_xml_xpointer(void) {
  cXMLXPointer = rb_define_class_under(mXML, "XPointer", rb_cObject);
  eXMLXPointerInvalidExpression = rb_define_class_under(cXMLXPointer, "InvalidExpression", eXMLError);

  rb_define_singleton_method(cXMLXPointer, "range", ruby_xml_xpointer_range, 2);
}
