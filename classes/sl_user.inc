<?php

module_load_include('inc', 'smart_helper', 'classes/sl_ya_object');

class sl_user extends sl_ya_object {

  public function __construct($base = NULL, $parent_object = NULL) {
    parent::__construct($base, $parent_object);
    
    // try to fill in the fb_uid if the ya_guid is specified
    if ((!isset($this->fb_uid) || empty($this->fb_uid)) && is_string($this->ya_guid) && !empty($this->ya_guid)) {
      if ($lookup = $this->load(array('ya_guid' => $this->ya_guid), TRUE)) {
        $this->fb_uid = $lookup->fb_uid;
      }
    }
  }
  
  public function __set($field, $value) {
    if ($field == 'fb_uid') {
      $this->$field = trim((string)$value);
      return;
    }
    else if ($field == 'fundraising_goal' || $field == 'amount_raised') {
      $value = (float)preg_replace('/[^0-9\.]/', '', (string)$value);
      $this->$field = (string)money_format('%!n', $value);
      return;
    }

    parent::__set($field, $value);
  }
  
  protected function table_name() {
    return SL_USERS_TABLE;
  }

  protected function primary_key_field() {
    return 'fb_uid';
  }
  
  protected function primary_key_type() {
    return 'string';
  }
  
  /** roots_translateable implementation **/
  function get_translations($src_type = NULL) {
    if ($src_type == ya_feed::get_type()) {
      return array(
        'FundraisingPage' => 'fundraising_url',
        'ID' => 'ya_guid',
        'Target' => 'fundraising_goal',
        'AmountRaised' => 'amount_raised',
        'LocationID' => 'location_ya_guid',
        'EventID' => 'event_ya_guid',
        // 'Name' => xxx @@TODO ??
      );
    }
  }
  
  public function insert() {
    if (!$this->fb_uid) {
      throw new Exception('User object cannot be inserted without a Facebook user ID.', SL_EXCEPTION_OBJECT_HAS_NO_KEYS);
    }
    parent::insert();
    
    // try to keep records in the sl_users_locations table of user locs
    if ($this->fb_uid && $this->ya_guid && $this->location_ya_guid) {
      $user_loc = new sl_user_location;
      $user_loc->fb_uid = $this->fb_uid;
      $user_loc->ya_guid = $this->ya_guid;
      $user_loc->location_guid = $this->location_ya_guid;
      try {
        if (!$user_loc->exists_in_db()) {
          $user_loc->insert();
        }
        else {
          $user_loc->update();
        }
      } catch (Exception $e) {
        // @@TODO: ??
      }
    }
  }
  
  public function update() {
    parent::update();
    
    // try to keep records in the sl_users_locations table of user locs
    if ($this->fb_uid && $this->ya_guid && $this->location_ya_guid) {
      $user_loc = new sl_user_location;
      $user_loc->fb_uid = $this->fb_uid;
      $user_loc->ya_guid = $this->ya_guid;
      $user_loc->location_guid = $this->location_ya_guid;
      try {
        if (!$user_loc->exists_in_db()) {
          $user_loc->insert();
        }
        else {
          $user_loc->update();
        }
      } catch (Exception $e) {
        // @@TODO: ??
      }
    }
  }
  
  /** roots_mockable implementation **/
  public function generate_mock_data($field) {
    switch ($field) {
      case 'fb_uid':
        if ($_GET['friends-of'] == 'bryn') {
          return array('5741', '13891', '20300', '20323', '20373', '22769', '108949', '309843', '601986', '617801', '619665', '822981', '827442', '836887', '908908', '919626');
        }
        else { // arthur
          return array(13132,23844,100341,100472,120429,300038,300683,307432,410889,416728,419669,506528,603915,604140,605932,608043,611711,706827,802235,802672,804413,812651,813803,815785,833053,844709,902494,905201,906857,1001077,1100057,1235966,1400046,1400076,1400246,1400312,1400353,1400522,1400523,1400530);
        }
      case 'fb_name':
        return array('Bryn Austin Bellomy', 'Arthur Shim', 'L. Ron Hubbard', 'David Keresh', 'Jim Jones', 'Rael', 'Dr. Gene Ray, Wisest Human', 'Aleister Crowley');
      case 'amount_raised':
        return range(0, 450);
      case 'fundraising_url':
        return array('http://yogaaid.com/brynbellomy');
      case 'ya_guid':
        /* random guids:
        $guids = array();
        foreach (range(1,10) as $i) {
          $guids[] = $this->make_uuid();
        }*/
        $guids = array('bf89458f-b78d-40cb-80ed-fb5525d0e1d5','8ea41edc-7682-4efd-b7df-1ce3bdc0a18b','c01e58f7-b278-4b97-bafb-e1671a1624f0','6a9c8a97-1f01-4ebd-af59-d3cf21cd6704','9409bfe3-4bb3-438e-a2c9-43c55b203947','ffcd838f-690f-462f-85cf-d0115ed9b6e5','caf41e6a-5b45-4aa9-a78a-1bf48f50fee3','563a3c66-01a7-404b-84fa-9d9a3aa7b066','bb516296-5007-403f-bad2-90cb2e8470e4','334c7666-1c0e-4c38-b714-ff9aa41d5e96','d09d34cd-c856-4fb6-ba4f-20e8f048aa60','3f687998-6b6d-4cf1-b301-6cfabbc3b0d2','161c424b-b215-4ceb-9373-d87e53b9c544','6999d2a5-de44-4ba1-a4fc-62d60465576c','1d1c5955-3298-45cf-8fcd-4609c1a1e066','62356f87-b8e3-4caf-821b-45dd4943aaa7','6227c113-c0b0-4941-9210-cd60345a47a6','f6c26308-94b5-4a78-81f8-d4e2ab487583','70941c43-dc09-4639-973e-db3e9b2c0bf1','bda3357b-bba8-45c4-9cbc-05986c123a8d','dae496e9-e002-44f8-9775-2f956d200968','8f842f5f-1cc5-4eb7-acd7-de57d7528010','40f62ee9-a655-4b67-b12f-9e4f4ef54947','b158af16-f19d-41ba-aa0a-a9a16ae15706','cbb3dae2-43dd-49c2-b890-82f0facdc0de','d48525b2-90f8-4e74-868d-0a2fd4fd2268','9a32cfa5-37e7-4432-a721-8aa7578894ca','69fed529-6a14-47c0-8845-137d8c02f02d','e476b944-40ef-432b-a750-1718fd729404','ef2f57ef-c6cb-4fd3-9cc9-8b7744832b97','a862ec68-becd-459f-a162-f948c6166072','8008caab-2c77-4e79-8f77-9c2dc1e486af','75876ece-38e8-4a7a-b87e-1f95f897bf1f','6c1b7a8e-e749-4e3e-a78e-08669090d4c4','ff6045b1-1cfd-472e-aa62-20a06812e696','469eade8-79e8-4887-9307-4c0c8d77e403','826a49ff-e667-4f22-b3b7-bda8fd366ef5','d3208b31-7531-4968-a20e-619c0495c36f','75334e4d-5d7a-405b-ada4-62aabe807619','21a0272c-21ca-48cf-a88a-61645b1034af','91116cc8-834d-4f5b-8386-15540056f99d','e8c4d2fa-a5e4-44e0-89e8-b9d3b8d39026','d5bd21f4-cdbd-4d1c-8190-89ec0bd1f228','d5bebb4a-d670-4ef2-b0c2-17b0ffd7f086','852a2f61-5c30-4711-b40c-9ae8c8030b95','e5f45ee7-47d1-4076-bf5e-9dd38a588d88','a39f05b0-88cc-4d5e-8b83-2664963cc03f','36a2f278-b4f2-471c-b9cc-f3b7dde9130a','b280899a-714e-4c24-b415-f73008c92954','cad7bd3d-559a-4b11-afcc-b5f6077b53a1','db1a4cf5-6132-48f4-8aee-5dc305bf6ca8','b164b5bb-83fa-403e-824c-4d5d1a1b6da2','c95db78f-9faa-4356-a95b-3dcb3eba8146','56b653f7-7021-4475-ad97-570ade9c9443','c706ebd8-9785-4280-bb53-100327056c4d','f9fb13c4-3113-4425-9200-8d66f4574cd4','154322de-fabd-4cf6-b6b8-84e5a093f41f','21fa63cd-942f-4585-a1db-b31e44cb3ee7','ae5305de-1a77-47f2-aa31-06316423795c','6d829bfd-d376-4f23-9771-37219e41caf8','3df2a5b9-d1c3-4e28-81b1-5e47ac18cab5','d3722ebe-c49e-4aa1-8e73-55efab28d787','61bd11fd-1126-47f3-8074-0d613e6fe96f','333dc875-be95-4599-8991-7cd8e2b08eb2','bad868ec-9c7b-48c3-8762-b9fec25155b0','b3076735-a42d-4f55-a0b1-4e1c0e617c55','eca10a83-ae50-49e4-b081-9bed9a3bd35a','9e866521-f777-4b81-8845-6bc32f5db135','a259c8b1-579c-4639-886e-a36382065f5b','c594b813-9dcd-406e-8d75-765b5c07ae4a','c2f3bf58-f134-4596-86f4-32e3ea0c9301','ec3831ea-786d-4482-a926-00a8f7b8946c','7c0ae9d1-d10f-434d-9bb9-e1d2c7c9ccbb','339fb92f-9ed8-476b-99de-8fbde4759b8f','926c46b3-5f23-4f23-88bf-fd1bb0b68e57','2d68dd2c-0a35-4a8a-9763-33c1862c809c','c26313b1-4a83-471e-96d9-944f6be1fa07','f875a0c0-55b6-4872-a7a7-d81862fbd6b8','9c42036b-941d-4088-8b07-3065eb8ddb50','4b5f7fd4-cc98-4073-a184-bf5651330d56','1667b448-4810-4954-a453-823522e8cab4','ecb7fc49-5a88-4dde-b543-65d6b3a7b686','ac60c197-8df5-4385-a1d5-48477aaa9ec2','ea03fd30-5ac3-4992-9dcc-5f1f115ef7d2','bd380041-641a-43ef-97ad-748a60fca250','9874bb4b-d140-4dca-88dc-04d33ab894af','9b5848b4-3244-4519-a196-cf6d9c763560','87c1ad1e-7f74-4358-aa0d-449c278fb7f2','e3f545ef-b13e-4528-b1bb-5576418a766d','83072dad-f92a-424b-ade7-aaf0be5d4f12');
        return $guids;
      case 'fundraising_goal':
        return range(500, 1000);
      case 'email':
        return array('bryn@rootdownmedia.com', 'arthur@rootdownmedia.com');
    }
    
    return parent::generate_mock_data($field); // default if unhandled field
  }
  
  /**
   * Generates an UUID
   * 
   * @author     Anis uddin Ahmad <admin@ajaxray.com>
   * @param      string  an optional prefix
   * @return     string  the formatted uuid
   */
  private function make_uuid($prefix = '') {
    $chars = uniqid(md5(mt_rand()), true);
    $uuid  = substr($chars,0,8) . '-';
    $uuid .= substr($chars,8,4) . '-';
    $uuid .= substr($chars,12,4) . '-';
    $uuid .= substr($chars,16,4) . '-';
    $uuid .= substr($chars,20,12);
    return $prefix . $uuid;
  }
}




/*  function translate_field($field_name, $data) {
    if ($field_name == 'ya_guid') {
      // try to find and fill in the fb_uid field using the ya_guid
//      if ($lookup = $this->load(array('ya_guid' => $data), TRUE)) {
//        $this->fb_uid = $lookup->fb_uid;
//        dsm($this, 'Found YA GUID');
//      }
//      else {
//        dsm($data, 'Could not find YA GUID in db');
//      }
    }
    
    return parent::translate_field($field_name, $data);
  }*/
  
  /*public function translate($data, $src_type = NULL) {
    $data = parent::translate($data, $src_type);
    
    // try to find and fill in the fb_uid field using the ya_guid
    if ($lookup = $this->load(array('ya_guid' => $data['ya_guid']), TRUE)) {
      $data['fb_uid'] = $lookup->fb_uid;
    }

    return $data;
  }*/