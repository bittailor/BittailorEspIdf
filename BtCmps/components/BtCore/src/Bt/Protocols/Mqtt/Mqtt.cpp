//*************************************************************************************************
//
//  BITTAILOR.CH - Bt::Protocols::Mqtt::Mqtt
//
//*************************************************************************************************

#include "Bt/Protocols/Mqtt/Mqtt.h"

#include <sstream>
#include <iterator>

namespace Bt {
namespace Protocols {
namespace Mqtt {
namespace {

bool matches(const char *sub, const char *topic)
{
   size_t spos;

   bool result = false;

   if(!sub || sub[0] == 0){
      throw std::invalid_argument("subscription can not be empty");
   }
   if(!sub || !topic || sub[0] == 0 || topic[0] == 0){
      throw std::invalid_argument("topic can not be empty");
   }

   if((sub[0] == '$' && topic[0] != '$') || (topic[0] == '$' && sub[0] != '$')){
      return result;
   }

   spos = 0;

   while(sub[0] != 0){
      if(topic[0] == '+' || topic[0] == '#'){
         throw std::invalid_argument("topic can not contain wildcards");
      }
      if(sub[0] != topic[0] || topic[0] == 0){ /* Check for wildcard matches */
         if(sub[0] == '+'){
            /* Check for bad "+foo" or "a/+foo" subscription */
            if(spos > 0 && sub[-1] != '/'){
               throw std::invalid_argument("bad + wildcards in subscription ('+foo' or 'a/+foo')");
            }
            /* Check for bad "foo+" or "foo+/a" subscription */
            if(sub[1] != 0 && sub[1] != '/'){
               throw std::invalid_argument("bad + wildcards in subscription ('foo+' or 'foo+/a')");
            }
            spos++;
            sub++;
            while(topic[0] != 0 && topic[0] != '/'){
               if(topic[0] == '+' || topic[0] == '#'){
                  throw std::invalid_argument("");
               }
               topic++;
            }
            if(topic[0] == 0 && sub[0] == 0){
               result = true;
               return result;
            }
         }else if(sub[0] == '#'){
            /* Check for bad "foo#" subscription */
            if(spos > 0 && sub[-1] != '/'){
               throw std::invalid_argument("bad # wildcards in subscription ('foo#')");
            }
            /* Check for # not the final character of the sub, e.g. "#foo" */
            if(sub[1] != 0){
               throw std::invalid_argument("bad # wildcards in subscription ('#foo')");
            }else{
               while(topic[0] != 0){
                  if(topic[0] == '+' || topic[0] == '#'){
                     throw std::invalid_argument("topic can not contain wildcards");
                  }
                  topic++;
               }
               result = true;
               return result;
            }
         }else{
            /* Check for e.g. foo/bar matching foo/+/# */
            if(topic[0] == 0
                  && spos > 0
                  && sub[-1] == '+'
                  && sub[0] == '/'
                  && sub[1] == '#')
            {
               result = true;
               return result;
            }

            /* There is no match at this point, but is the sub invalid? */
            while(sub[0] != 0){
               if(sub[0] == '#' && sub[1] != 0){
                  throw std::invalid_argument("bad # wildcards in subscription ('#foo')");
               }
               spos++;
               sub++;
            }

            /* Valid input, but no match */
            return result;
         }
      }else{
         /* sub[spos] == topic[tpos] */
         if(topic[1] == 0){
            /* Check for e.g. foo matching foo/# */
            if(sub[1] == '/'
                  && sub[2] == '#'
                  && sub[3] == 0){
               result = true;
               return result;
            }
         }
         spos++;
         sub++;
         topic++;
         if(sub[0] == 0 && topic[0] == 0){
            result = true;
            return result;
         }else if(topic[0] == 0 && sub[0] == '+' && sub[1] == 0){
            if(spos > 0 && sub[-1] != '/'){
               throw std::invalid_argument("bad + wildcards in subscription ('+foo' or 'a/+foo')");
            }
            spos++;
            sub++;
            result = true;
            return result;
         }
      }
   }
   if((topic[0] != 0 || sub[0] != 0)){
      result = false;
   }
   while(topic[0] != 0){
      if(topic[0] == '+' || topic[0] == '#'){
         throw std::invalid_argument("topic can not contain wildcards");
      }
      topic++;
   }

   return result;
}
}


std::vector<std::string> split(const std::string& pTopic)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(pTopic);
   while (std::getline(tokenStream, token, TOPIC_SEPARATOR))
   {
      tokens.push_back(token);
   }
   return tokens;
}

std::string join(const std::vector<std::string>& pParts) {
   if(pParts.empty()) {
      return {};
   }
   std::ostringstream stream;
   for(size_t i = 0 ; i < pParts.size()-1 ; i++) {
      stream << pParts[i] << TOPIC_SEPARATOR;
   }
   if(pParts.size() > 0) {
      stream << pParts[pParts.size()-1];
   }
   return stream.str();
}

bool topicMatchesSubscription(const std::string& pSubscription, const std::string& pTopic) {
   return matches(pSubscription.c_str(), pTopic.c_str());
}

} // namespace Mqtt
} // namespace Protocols
} // namespace Bt
