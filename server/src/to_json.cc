#include "../include/to_json.h"
#include "../include/connection.h"
#include "../include/endpoint.h"
#include "../include/websocketSSL.h"
#include "../include/display_format.h"
#include <iostream>

ToJSON::SortMethod ToJSON::sort_method_ = ToJSON::NORMAL;

bool cmp_cpu_first(const ProcessInfo &lhs, const ProcessInfo &rhs){
  return lhs.cpu > rhs.cpu;
}

bool cmp_mem_first(const ProcessInfo &lhs, const ProcessInfo &rhs){
  return lhs.mem > rhs.mem;
}

ToJSON::ToJSON(){
  procs_info_.clear();
}

void ToJSON::set_overallinfo(OverallInfo &overallinfo){
  overallinfo_ = overallinfo;
}

void ToJSON::set_processinfo(std::list<ProcessInfo> &processinfo){
  procs_info_.swap(processinfo);
  switch (sort_method_) {
    case CPU_FIRST:
      procs_info_.sort(cmp_cpu_first);
      break;
    case MEM_FIRST:
      procs_info_.sort(cmp_mem_first);
      break;
    default:
      break;
  }
}

// void ToJSON::data_to_json(std::shared_ptr<asio::streambuf> &write_buffer){
//   std::ostream stream(write_buffer.get());
//   stream << "{\"users\":" << overallinfo_.users << ", \"total\":" << overallinfo_.total;
//   stream << ", \"running\":" << overallinfo_.running << ", \"sleeping\":" << overallinfo_.sleeping;
//   stream << ", \"stopped\":" << overallinfo_.stopped << ", \"zombie\":" << overallinfo_.zombie;
//   stream << ", \"mem_total\":" << overallinfo_.mem_total << ", \"mem_free\":" << overallinfo_.mem_free;
//   stream << ", \"mem_used\":" << overallinfo_.mem_used << ", \"buffer_cached\":" << overallinfo_.buffer_cached;
//   stream << ", \"swap_total\":" << overallinfo_.swap_total << ", \"swap_free\":" << overallinfo_.swap_free;
//   stream << ", \"swap_used\":" << overallinfo_.swap_used << ", \"avail\":" << overallinfo_.avail;
//   stream << ", \"avg_one\":" << overallinfo_.avg_one << ", \"avg_five\":" << overallinfo_.avg_five;
//   stream << ", \"avg_fiftheen\":" << overallinfo_.avg_fiftheen;
//
//   int proc_size = procs_info_.size();
//   int count = 0;
//   stream << ",\"processinfo\":[";
//   for(auto proc : procs_info_){
//     stream << "{\"state\":\"" << proc.state << "\", \"pid\":" << proc.pid;
//     stream << ", \"ppid\":" << proc.ppid << ", \"userid\":" << proc.userid;
//     stream << ", \"pmem\":" << proc.pmem << ", \"vmem\":" << proc.vmem;
//     stream << ", \"cpu\":" << proc.cpu << ", \"mem\":" << proc.mem;
//     stream << ", \"cmdline\":\"" << proc.cmdline << "\"}";
//     ++count < proc_size ? stream << "," : stream << "]}";
//   }
// }

void ToJSON::notify_send_data(){
  Endpoint *endpoint = Endpoint::get_instance();
  std::unordered_set<std::shared_ptr<WebSocketSSL>> connections = endpoint->get_connections();
  if(connections.empty())
    return;

  for(auto &client : connections){
    std::shared_ptr<asio::streambuf> write_buffer = std::make_shared<asio::streambuf>();
    client->get_connection()->display_format_->data_to_json(write_buffer, overallinfo_, procs_info_);
    client->send(write_buffer);
  }
}
